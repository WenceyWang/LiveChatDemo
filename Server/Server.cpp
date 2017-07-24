// ConsoleApplication1.cpp : main project file.

#pragma once

#include "Source.hpp"
#include "stdafx.h"

namespace WenceyWang {

	namespace LiveChatDemo
	{

		namespace Server {

			public ref class UserLoginFailedException :Exception
			{
			public:
				String ^Name;
				IPAddress^ Sender;

				UserLoginFailedException(String^ name, IPAddress^ sender)
				{
					Name = name;
					Sender = sender;
				}

			};

			public ref class UserNotFoundException :Exception
			{
			public:
				String ^Name;

				UserNotFoundException(String^ name)
				{
					Name = name;
				}

				virtual String^ ToString() override
				{
					return String::Format("Group {0} Not Found", Name);
				}

			};

			public ref class App :Application
			{
			public:

				UdpClient^ Listener;
				Thread^ ListenThread;
				Thread^ SendThread;
				Thread^ ProcessThread;

				static int Port = 3344;

				App()
				{
					Current = this;
					Sender = gcnew UdpClient();
					Users = gcnew List<User^>();
					Groups = gcnew List<Group^>();
					InMessage = gcnew Queue<ClientPackage^>();
					OutMessage = gcnew Queue<ServerPackage^>();
				}


				UdpClient^ Sender;

				List<User^>^ Users;

				List<Group^>^ Groups;

				Queue<ClientPackage^>^ InMessage;

				Queue<ServerPackage^>^ OutMessage;

				void SaveUserList()
				{
					for each (User^ user in Users)
					{
						//Todo:finish this
					}
				}


				void SendPackage(ServerPackage ^package)
				{
					lock l(OutMessage);
					OutMessage->Enqueue(package);
					l.release();

				}

				void Listen()
				{

					LogInfo("Bind to Port {0}", Port);
					Listener = gcnew UdpClient(gcnew IPEndPoint(IPAddress::Any, Port));
					LogInfo("Bind Success");

					LogInfo("Search Package Types");

					array<Type^>^ allTypes = this->GetType()->Assembly->GetTypes();
					array<Type^>^ types = Array::FindAll(allTypes, gcnew Predicate<Type^>(ClientPackage::ChooseClientPackageType));

					LogInfo("Start Lisining");

					while (true)
					{
						try {

							IPEndPoint^ endPoint = gcnew IPEndPoint(IPAddress::Any, Port);
							array<unsigned char>^ value = Listener->Receive(endPoint);
							XElement^ element = InterOp::ToXElement(value);

							//LogDebug("Receive package {0} from {1}", element, endPoint->Address);

							TypeNamePredicate ^ namePred = gcnew TypeNamePredicate(element->Name->ToString());

							Type ^type = Array::Find(types, gcnew Predicate<Type^>(namePred, &TypeNamePredicate::ChooseName));


							if (type == nullptr)
							{
								throw gcnew NotSupportedException(String::Format("{0} class not found", element->Name));
							}

							ClientPackage^ package = (ClientPackage^)Activator::CreateInstance(type, endPoint->Address, element);

							lock l(InMessage);

							InMessage->Enqueue(package);

							l.release();


						}
						catch (Exception^ e)
						{
							LogWarn(e->ToString());
						}

					}
				}

				void Process()
				{

					LogInfo("Start Process");

					while (true)
					{
						try {

							lock l(InMessage);
							if (InMessage->Count > 0)
							{
								ClientPackage^ toProcess = InMessage->Dequeue();
								l.release();
								toProcess->Process();
							}
							else
							{
								l.release();
								Thread::Sleep(500);
							}
						}
						catch (UserLoginFailedException ^e)
						{
							LogInfo("{0} Log {1} Failed", e->Sender, e->Name);
						}
						catch (Exception^ e)
						{
							LogWarn(e->ToString());
						}
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
							ServerPackage^ toSent = OutMessage->Dequeue();
							l.release();
							XElement^ element = toSent->ToXElement();
							array<unsigned char>^ bytes = InterOp::ToByte(element->ToString());
							Sender->Send(bytes, bytes->Length, gcnew IPEndPoint(toSent->Target, Port));
						}
						else
						{
							l.release();
							Thread::Sleep(500);
						}

					}
				}


				static App ^Current;


				virtual	void Start() override
				{

					LogInfo("Server Starting");

					LogInfo("Starting Lisiten Thread");
					ListenThread = gcnew Thread(gcnew ThreadStart(this, &App::Listen));
					ListenThread->Start();
					LogInfo("Lisiten Thread Started");

					LogInfo("Starting Send Thread");
					SendThread = gcnew Thread(gcnew ThreadStart(this, &App::Sending));
					SendThread->Start();
					LogInfo("Send Thread Started");

					LogInfo("Starting Process Thread");
					ProcessThread = gcnew Thread(gcnew ThreadStart(this, &App::Process));
					ProcessThread->Start();
					LogInfo("Process Thread Started");

				}

			};
		}

		User^ GetUser(String^ name)
		{
			UserNamePredicate^ pred = gcnew UserNamePredicate(name);
			User^user = Server::App::Current->Users->Find(gcnew Predicate<User^>(pred, &UserNamePredicate::ChooseName));
			if (user == nullptr)
			{
				throw gcnew Server::UserNotFoundException(name);
			}
			return user;
		}

		User^ GetUser(Guid^ guid)
		{
			UserGuidPredicate^ pred = gcnew UserGuidPredicate(guid);
			User^user = Server::App::Current->Users->Find(gcnew Predicate<User^>(pred, &UserGuidPredicate::ChooseGuid));
			return user;
		}


		User^ GetSender(ClientPackage^ package)
		{
			return GetUser(package->CurrentLoginInfo->Name);
		}

		Group^ GetGroup(String^ name)
		{
			GroupNamePredicate^ pred = gcnew GroupNamePredicate(name);
			Group^group = Server::App::Current->Groups->Find(gcnew Predicate<Group^>(pred, &GroupNamePredicate::ChooseName));
			if (group == nullptr)
			{
				throw gcnew Server::UserNotFoundException(name);
			}
			return group;
		}

		User^ GetUserWithIndex(Guid guid, int index)
		{
			UserGuidPredicate^ pred = gcnew UserGuidPredicate(guid);
			User^user = Server::App::Current->Users->Find(gcnew Predicate<User^>(pred, &UserGuidPredicate::ChooseGuid));
			return user;
		}


		void WenceyWang::LiveChatDemo::ClientPackage::Process()
		{
			User^user;
			try
			{
				user = GetSender(this);
			}
			catch (Server::UserNotFoundException ^)
			{
				throw gcnew Server::UserLoginFailedException(this->CurrentLoginInfo->Name, this->Source);
			}
			if (user->CheckLoginInfo(this->CurrentLoginInfo))
			{
			}
			else
			{
				throw gcnew Server::UserLoginFailedException(user->Name, this->Source);
			}
		}

		void WenceyWang::LiveChatDemo::SendMessagePackage::Process()
		{
			ClientPackage::Process();
			User^sender = GetSender(this);
			User^receiver = GetUser(this->TargetUser);
			if (!receiver->Blockeds->Contains(sender))
			{
				lock l(receiver);
				receiver->Messages->Enqueue(gcnew MessagePackage(sender->Name, this->Content));
				l.release();
			}
		}

		void WenceyWang::LiveChatDemo::GetMessagesPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			lock l(user);
			user->LastSeen = DateTime::UtcNow;
			while (user->Messages->Count > 0)
			{
				ServerPackage^ package = user->Messages->Dequeue();
				package->Target = this->Source;
				(Server::App::Current)->SendPackage(package);
			}
			l.release();
		}

		void WenceyWang::LiveChatDemo::GetUsersPackage::Process()
		{
			ClientPackage::Process();
			ReturnUsersPackage^ package = gcnew ReturnUsersPackage(Enumerable::ToList(Enumerable::Select((Server::App::Current->Users), gcnew Func<User^, int, UserInfo^>(User::ToUserInfo))), this->Source);
			(Server::App::Current)->SendPackage(package);
		}

		void WenceyWang::LiveChatDemo::GetFriendsPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			ReturnFriendsPackage^ package = gcnew ReturnFriendsPackage(Enumerable::ToList(Enumerable::Select(user->Friends, gcnew Func<User^, int, UserInfo^>(User::ToUserInfo))), this->Source);
			(Server::App::Current)->SendPackage(package);
		}

		void WenceyWang::LiveChatDemo::GetBlockedPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			ReturnBlockedPackage^ package = gcnew ReturnBlockedPackage(Enumerable::ToList(Enumerable::Select(user->Blockeds, gcnew Func<User^, int, UserInfo^>(User::ToUserInfo))), this->Source);
			(Server::App::Current)->SendPackage(package);
		}

		void WenceyWang::LiveChatDemo::AddFriendPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			User^Friend = GetUser(this->TargetUser);
			user->Friends->Add(Friend);
		}

		void WenceyWang::LiveChatDemo::RegisAccountPackage::Process()
		{
			//No logininfo check
			Server::App::Current->Users->Add(gcnew User(Guid::NewGuid(), this->Name, this->Password));
			Server::App::Current->LogInfo("User {0} regised", this->Name);
		}

		void WenceyWang::LiveChatDemo::BlockUserPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			User^blocked = GetUser(this->TargetUser);
			user->Blockeds->Add(blocked);
		}

		void WenceyWang::LiveChatDemo::UnblockUserPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			User^blocked = GetUser(this->TargetUser);
			user->Blockeds->Remove(blocked);
		}

		void WenceyWang::LiveChatDemo::RemoveFriendPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			User^Friend = GetUser(this->TargetUser);
			user->Friends->Remove(Friend);
		}

		void WenceyWang::LiveChatDemo::GroupRemoveUserPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			Group^ group = GetGroup(this->TargetGroup);
			User^ toRemove = GetUser(this->TargetUser);
			if (group->Owner == user)
			{
				lock l(group);

				group->Users->Remove(toRemove);
				if (!group->Users->Contains(group->Owner))
				{
					Server::App::Current->Groups->Remove(group);
				}
				l.release();
			}


		}

		void WenceyWang::LiveChatDemo::GroupAddUserPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			Group^ group = GetGroup(this->TargetGroup);
			User^ toAdd = GetUser(this->TargetUser);
			if (group->Owner == user && !toAdd->Blockeds->Contains(user))
			{
				lock l(group);
				group->Users->Add(toAdd);
				l.release();
			}

		}

		void WenceyWang::LiveChatDemo::CreateGroupPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSender(this);
			Group^ newGroup = gcnew Group(this->Name, user);

			Server::App::Current->Groups->Add(newGroup);

		}

		void WenceyWang::LiveChatDemo::SendGroupMessagePackage::Process()
		{
			ClientPackage::Process();
			User^sender = GetSender(this);
			Group^ group = GetGroup(this->TargetGroup);
			if (group->Users->Contains(sender))
			{
				for each (User^ receiver in group->Users)
				{
					lock l(receiver);
					receiver->Messages->Enqueue(gcnew MessagePackage(sender->Name, this->Content));
					l.release();

				}
			}
		}

		void WenceyWang::LiveChatDemo::GetGroupUsersPackage::Process()
		{
			ClientPackage::Process();
			User^sender = GetSender(this);
			Group^ group = GetGroup(this->TargetGroup);
			if (group->Users->Contains(sender))
			{
				ReturnGroupUsersPackage^ package = gcnew ReturnGroupUsersPackage(Enumerable::ToList(Enumerable::Select(group->Users, gcnew Func<User^, int, UserInfo^>(User::ToUserInfo))), this->Source);
				(Server::App::Current)->SendPackage(package);
			}
		}

	}
}


int main(array<System::String ^> ^args)
{
	Console::WriteLine("DGZ Chat Copyright(C) 2017 Wencey Wang");
	Console::WriteLine("This program comes with ABSOLUTELY NO WARRANTY;");
	Console::WriteLine("This is free software, and you are welcome to redistribute it under certain conditions;");
	Console::WriteLine("Visit https://www.gnu.org/licenses/agpl-3.0.html for details.");

	Console::WriteLine();

	WenceyWang::LiveChatDemo::Server::App^ App = gcnew WenceyWang::LiveChatDemo::Server::App();

	App->Start();

	return 0;
}

