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
			

			public ref class App:Application
			{
				IPEndPoint^ Server;
				Thread^ ListenThread;
				Thread^ SendThread;
				Thread^CheckNewMessageThread;
				UdpClient^ Listener;
				LoginInfo^ UserLoginInfo;
				UdpClient^ Sender = gcnew UdpClient();


				Queue<ClientPackage^>^ OutMessage;

			public:

				App()
				{

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
						GetMessagesPackage ^ package = gcnew GetMessagesPackage(Server, this->UserLoginInfo);
						this->SendPackage(package);
						Thread::Sleep(1000);
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
							Sender->Send(bytes, bytes->Length, toSent->Target);
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
					Listener = gcnew UdpClient(Server);
					LogInfo("Bind Success");

					LogInfo("Search Package Types");
					array<Type^>^ types = Array::FindAll(this->GetType()->Assembly->GetTypes(), gcnew Predicate<Type^>(ServerPackage::ChooseServerPackageType));

					LogInfo("Start Lisining");
					while (true)
					{
						IPEndPoint^ address = gcnew IPEndPoint(Server->Address,Server->Port);
						array<unsigned char>^ value = Listener->Receive(address);
						XElement^ element = InterOp::ToXElement(value);

						TypeNamePredicate ^ namePred = gcnew TypeNamePredicate(element->Name->ToString());

						Type ^type = Array::Find(types, gcnew Predicate<Type^>(namePred, &TypeNamePredicate::ChooseName));

						ServerPackage^ package = (ServerPackage^)Activator::CreateInstance(type, address->Address, element);

					}
				}


				void Start() override

				{
					Console::WriteLine("Enter Server Address and port:");

					String ^ addressString= Console::ReadLine();

					Server = InterOp::Parse(addressString);

					Console::WriteLine("Enter UserName");
					String ^ name = Console::ReadLine();
					Console::WriteLine("Enter Password");
					String ^ password = Console::ReadLine();

					UserLoginInfo = gcnew LoginInfo(name, password);

					LogInfo("Starting Lisiten Thread");
					ListenThread = gcnew Thread(gcnew ThreadStart(this, &App::Listening));
					ListenThread->Start();
					LogInfo("Lisiten Thread Started");

					LogInfo("Starting Sending Thread");
					SendThread = gcnew Thread(gcnew ThreadStart(this, &App::Sending));
					SendThread->Start();
					LogInfo("Sending thread Started");

					CheckNewMessageThread = gcnew Thread(gcnew ThreadStart(this, &App::CheckNewMessage));
					CheckNewMessageThread->Start();



				}

			};
		}

		void WenceyWang::LiveChatDemo::ClientPackage::Process()
		{
		}

		void WenceyWang::LiveChatDemo::SendMessagePackage::Process()
		{
		}

		void WenceyWang::LiveChatDemo::GetMessagesPackage::Process()
		{
		}

		void WenceyWang::LiveChatDemo::GetUsersPackage::Process()
		{
		}

		void WenceyWang::LiveChatDemo::GetFriendsPackage::Process()
		{
		}

		void WenceyWang::LiveChatDemo::AddFriendPackage::Process()
		{
		}

	}
}


int main(array<System::String ^> ^args)
{
	WenceyWang::LiveChatDemo::Client::App^ App = gcnew WenceyWang::LiveChatDemo::Client::App();

	App->Start();

	return 0;
}
