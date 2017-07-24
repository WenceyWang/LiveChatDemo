// Client.cpp : main project file.

#include "Source.hpp"
#include "stdafx.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Xml::Linq;
using namespace System::Linq;
using namespace System::Text;
using namespace  System::Security::Cryptography;
using namespace msclr;

//I should use FoggyConsole To drow UI But the Time is not enough ┑(￣Д ￣)┍

namespace WenceyWang {

	namespace LiveChatDemo
	{
		namespace Client {

			public ref class Command abstract
			{
			public:

				virtual	void Excute(array<System::String ^> ^args) abstract;

				virtual	String^ GetHelp()
				{
					return "No help aviliable.";
				}

				static bool ChooseCommandType(Type^ type)
				{
					Type ^t = Command::typeid;
					return t->IsAssignableFrom(type) && (!type->IsAbstract);
				}
			};





			public ref class App :Application
			{
			public:


				IPEndPoint^ Server;
				Thread^ ListenThread;
				Thread^ SendThread;
				Thread^CheckNewMessageThread;
				Thread^CommandThread;
				UdpClient^ Listener;
				LoginInfo^ UserLoginInfo;
				UdpClient^ Sender = gcnew UdpClient();

				int CheckMessageInterval = 1000;

				static App^ Current;


				Queue<ClientPackage^>^ OutMessage = gcnew Queue<ClientPackage^>();



				App()
				{
					Current = this;
				}

				void SendPackage(ClientPackage ^package)
				{
					lock l(OutMessage);
					OutMessage->Enqueue(package);
					l.release();

				}

				void CheckNewMessage()
				{
					while (true)
					{
						GetMessagesPackage ^ package = gcnew GetMessagesPackage(Server->Address, this->UserLoginInfo);
						this->SendPackage(package);
						Thread::Sleep(CheckMessageInterval);
					}
				}

				void Sending()
				{
					LogInfo("Start Sending");

					while (true)
					{
						lock l(OutMessage);
						if (OutMessage->Count > 0)
						{
							ClientPackage^ toSent = OutMessage->Dequeue();
							l.release();
							XElement^ element = toSent->ToXElement();
							array<unsigned char>^ bytes = InterOp::ToByte(element->ToString());
							Sender->Send(bytes, bytes->Length, Server);
						}
						else
						{
							l.release();
							Thread::Sleep(500);
						}
					}
				}

				void Listening()
				{
					LogInfo("Bind to Port {0}", Server->Port);
					Listener = gcnew UdpClient(Server->Port);
					LogInfo("Bind Success");

					LogInfo("Search Package Types");
					array<Type^>^ types = Array::FindAll(this->GetType()->Assembly->GetTypes(), gcnew Predicate<Type^>(ServerPackage::ChooseServerPackageType));

					LogInfo("Start Lisining");
					while (true)
					{
						try {

							IPEndPoint^ address = gcnew IPEndPoint(Server->Address, Server->Port);
							array<unsigned char>^ value = Listener->Receive(address);
							XElement^ element = InterOp::ToXElement(value);

							TypeNamePredicate ^ namePred = gcnew TypeNamePredicate(element->Name->ToString());

							Type ^type = Array::Find(types, gcnew Predicate<Type^>(namePred, &TypeNamePredicate::ChooseName));

							if (type == nullptr)
							{
								throw gcnew NotSupportedException(String::Format("{0} class not found", element->Name));
							}

							ServerPackage^ package = (ServerPackage^)Activator::CreateInstance(type, address->Address, element);

							Console::WriteLine(package->ToString());
						}
						catch (Exception^ e)
						{
							LogWarn(e->ToString());
						}

					}
				}


				static array<String^>^ SplitArguments(String^ commandLine)
				{
					array<wchar_t>^ parmChars = commandLine->ToCharArray();
					bool inSingleQuote = false;
					bool inDoubleQuote = false;
					for (int index = 0; index < parmChars->Length; index++)
					{
						if (parmChars[index] == '"' && !inSingleQuote)
						{
							inDoubleQuote = !inDoubleQuote;
							parmChars[index] = '\n';
						}
						if (parmChars[index] == '\'' && !inDoubleQuote)
						{
							inSingleQuote = !inSingleQuote;
							parmChars[index] = '\n';
						}
						if (!inSingleQuote && !inDoubleQuote && parmChars[index] == ' ')
							parmChars[index] = '\n';
					}
					return (gcnew String(parmChars))->Split((gcnew String("\n"))->ToCharArray(), StringSplitOptions::RemoveEmptyEntries);
				}


				void ExcuteCommand()
				{
					LogInfo("Starting Commanding");

					array<Type^>^ types = Array::FindAll(this->GetType()->Assembly->GetTypes(), gcnew Predicate<Type^>(Command::ChooseCommandType));


					while (true)
					{
						try
						{
							/*{
								lock l(ConsoleLocker);
								int cursorLeft = Console::CursorLeft;
								int cursorTop = Console::CursorTop;
								Console::CursorLeft = 0;
								Console::CursorTop = Console::WindowHeight - 1;
								Console::Write(">");
								Console::CursorLeft = cursorLeft;
								Console::CursorTop = cursorTop;
								l.release();
							}*/

							array<String^>^ currentCommand = SplitArguments(Console::ReadLine());

							TypeNamePredicate ^ namePred = gcnew TypeNamePredicate(currentCommand[0]);

							Type ^type = Array::Find(types, gcnew Predicate<Type^>(namePred, &TypeNamePredicate::ChooseName));

							if (type == nullptr)
							{
								throw gcnew NotSupportedException(String::Format("{0} command not found", currentCommand[0]));
							}

							Command^ command = (Command^)Activator::CreateInstance(type);

							command->Excute(currentCommand);


						}
						catch (Exception^ e)
						{
							LogWarn(e->ToString());
						}
					}

				}

				void Start() override
				{


					Console::WriteLine("Enter Server Address and port:");

					String ^ addressString = Console::ReadLine();

					Server = InterOp::Parse(addressString);

					Console::WriteLine("Login(L) or Regis(R)");

					String^ loginOrRegis = Console::ReadLine();

					Console::WriteLine("Enter UserName");
					String ^ name = Console::ReadLine();
					Console::WriteLine("Enter Password");
					String ^ password = Console::ReadLine();

					UserLoginInfo = gcnew LoginInfo(name, password);

					if (loginOrRegis == "R")
					{
						RegisAccountPackage ^ package = gcnew RegisAccountPackage(name, password, Server->Address, UserLoginInfo);
						OutMessage->Enqueue(package);
					}

					LogInfo("Starting Lisiten Thread");
					ListenThread = gcnew Thread(gcnew ThreadStart(this, &App::Listening));
					ListenThread->Start();
					LogInfo("Lisiten Thread Started");

					LogInfo("Starting Sending Thread");
					SendThread = gcnew Thread(gcnew ThreadStart(this, &App::Sending));
					SendThread->Start();
					LogInfo("Sending thread Started");

					LogInfo("Starting CheckNewMessage Thread");
					CheckNewMessageThread = gcnew Thread(gcnew ThreadStart(this, &App::CheckNewMessage));
					CheckNewMessageThread->Start();
					LogInfo("CheckNewMessage thread started");

					LogInfo("Starting Command Thread");
					CommandThread = gcnew Thread(gcnew ThreadStart(this, &App::ExcuteCommand));
					CommandThread->Start();
					LogInfo("Command Thread Started");

				}

			};

			public ref class CreateGroup :Command
			{
			public:

				virtual String^ GetHelp() override
				{
					return "CreateGroup GROUPNAME...";
				}

				void Excute(array<System::String ^> ^args) override
				{
					CreateGroupPackage^ package = gcnew CreateGroupPackage(args[1], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class GroupAddUser :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					GroupAddUserPackage^ package = gcnew GroupAddUserPackage(args[1], args[2], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}

			};

			public ref class GroupRemoveUser :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					GroupRemoveUserPackage^ package = gcnew GroupRemoveUserPackage(args[1], args[2], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class SendGroupMessage :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					SendGroupMessagePackage^ package = gcnew SendGroupMessagePackage(args[1], args[2], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class AddFriend :Command
			{
			public:

				void Excute(array<System::String ^> ^args) override
				{
					AddFriendPackage^ package = gcnew AddFriendPackage(args[1], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class RemoveFriend :Command
			{
			public:

				void Excute(array<System::String ^> ^args) override
				{
					RemoveFriendPackage^ package = gcnew RemoveFriendPackage(args[1], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class UnblockUser :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					UnblockUserPackage^ package = gcnew UnblockUserPackage(args[1], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};


			public ref class GetFriends :Command
			{
			public:
				void Excute(array<String^>^ args)override
				{
					GetFriendsPackage^ package = gcnew GetFriendsPackage(App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}

			};

			public ref class SendMessage :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					SendMessagePackage^ package = gcnew SendMessagePackage(args[1], args[2], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class GetGroupUsers :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					GetGroupUsersPackage^ package = gcnew GetGroupUsersPackage(args[1], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class GetBlocked :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					GetBlockedPackage^ package = gcnew GetBlockedPackage(App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class GetUsers :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					GetUsersPackage^ package = gcnew GetUsersPackage(App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class SetCheckMessageInterval :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					App::Current->CheckMessageInterval = Convert::ToInt32(args[1]);
				}
			};

			public ref class BlockUser :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					BlockUserPackage^ package = gcnew BlockUserPackage(args[1], App::Current->Server->Address, App::Current->UserLoginInfo);
					App::Current->SendPackage(package);
				}
			};

			public ref class Help :Command
			{
			public:
				void Excute(array<System::String ^> ^args) override
				{
					array<Type^>^ types = Array::FindAll(this->GetType()->Assembly->GetTypes(), gcnew Predicate<Type^>(Command::ChooseCommandType));

					switch (args->Length)
					{
					case 1:
					{
						lock l(Client::App::Current->ConsoleLocker);
						for each (Type^ type in types)
						{
							Console::WriteLine(type->Name);
						}
						l.release();

						break;
					}
					case 2:
					{
						TypeNamePredicate ^ namePred = gcnew TypeNamePredicate(args[1]);

						Type ^type = Array::Find(types, gcnew Predicate<Type^>(namePred, &TypeNamePredicate::ChooseName));

						if (type == nullptr)
						{
							throw gcnew NotSupportedException(String::Format("{0} command not found", args[1]));
						}

						Command^ command = (Command^)Activator::CreateInstance(type);

						Console::WriteLine(command->GetHelp());

					}
					default:
					{
						break;
					}
					}
				}
			};


		}

		void WenceyWang::LiveChatDemo::ClientPackage::Process() {}
		void WenceyWang::LiveChatDemo::SendMessagePackage::Process() {}
		void WenceyWang::LiveChatDemo::GetMessagesPackage::Process() {}
		void WenceyWang::LiveChatDemo::GetUsersPackage::Process() {}
		void WenceyWang::LiveChatDemo::GetFriendsPackage::Process() {}
		void WenceyWang::LiveChatDemo::AddFriendPackage::Process() {}
		void WenceyWang::LiveChatDemo::RegisAccountPackage::Process() {}
		void WenceyWang::LiveChatDemo::BlockUserPackage::Process() {}
		void WenceyWang::LiveChatDemo::SendGroupMessagePackage::Process() {}
		void WenceyWang::LiveChatDemo::CreateGroupPackage::Process() {}
		void WenceyWang::LiveChatDemo::GroupAddUserPackage::Process() {}
		void WenceyWang::LiveChatDemo::GroupRemoveUserPackage::Process() {}
		void WenceyWang::LiveChatDemo::GetGroupUsersPackage::Process() {}
		void WenceyWang::LiveChatDemo::GetBlockedPackage::Process() {}
		void WenceyWang::LiveChatDemo::UnblockUserPackage::Process() {}
		void WenceyWang::LiveChatDemo::RemoveFriendPackage::Process() {}
	}
}


int main(array<System::String ^> ^args)
{
	WenceyWang::LiveChatDemo::Client::App^ App = gcnew WenceyWang::LiveChatDemo::Client::App();

	App->Start();

	return 0;
}
