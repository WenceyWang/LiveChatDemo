// ConsoleApplication1.cpp : main project file.

#pragma once

#include "Source.hpp"
#include "stdafx.h"

namespace WenceyWang {

	namespace LiveChatDemo
	{


		namespace Server {

			public ref class App :Application
			{
			public:

				UdpClient^ Listener;
				Thread^ ListenThread;
				Thread^ SendThread;
				Thread^ ProcessThread;

				int Port = 3344;

				UdpClient^ Sender = gcnew UdpClient();

				List<User^>^ Users;

				Queue<ClientPackage^>^ InMessage = gcnew Queue<ClientPackage^>();

				Queue<ServerPackage^>^ OutMessage = gcnew Queue<ServerPackage^>();

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


							IPEndPoint^ address = gcnew IPEndPoint(IPAddress::Any, 3344);
							array<unsigned char>^ value = Listener->Receive(address);
							XElement^ element = InterOp::ToXElement(value);

							TypeNamePredicate ^ namePred = gcnew TypeNamePredicate(element->Name->ToString());

							Type ^type = Array::Find(types, gcnew Predicate<Type^>(namePred, &TypeNamePredicate::ChooseName));


							if (type == nullptr)
							{
								throw gcnew NotSupportedException(String::Format("{0} class not found", element->Name));
							}

							ClientPackage^ package = (ClientPackage^)Activator::CreateInstance(type, address->Address, element);

							lock l(InMessage);

							InMessage->Enqueue(package);

							l.release();


						}
						catch (Exception^ e)
						{
							Console::WriteLine(e->ToString());
						}

					}
				}

				void Process()
				{

					LogInfo("Start Process");

					while (true)
					{
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
							Sender->Send(bytes, bytes->Length, toSent->Target);
						}
						else
						{
							l.release();
							Thread::Sleep(500);
						}

					}
				}


				static App ^Current;

				App()
				{
					Current = this;
				}

				void Start() override
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
					ProcessThread = gcnew Thread(gcnew ThreadStart(this, &App::Sending));
					ProcessThread->Start();
					LogInfo("Process Thread Started");

				}

			};
		}

		User^ GetSendUser(ClientPackage^ package)
		{
			UserNamePredicate^ pred = gcnew UserNamePredicate(package->CurrentLoginInfo->Name);
			User^user = Server::App::Current->Users->Find(gcnew Predicate<User^>(pred, &UserNamePredicate::ChooseName));
			return user;
		}

		User^ GetUser(Guid^ guid)
		{
			UserGuidPredicate^ pred = gcnew UserGuidPredicate(guid);
			User^user = Server::App::Current->Users->Find(gcnew Predicate<User^>(pred, &UserGuidPredicate::ChooseGuid));
			return user;
		}

		User^ GetUserWithIndex(Guid guid, int index)
		{
			UserGuidPredicate^ pred = gcnew UserGuidPredicate(guid);
			User^user = Server::App::Current->Users->Find(gcnew Predicate<User^>(pred, &UserGuidPredicate::ChooseGuid));
			return user;
		}


		void WenceyWang::LiveChatDemo::ClientPackage::Process()
		{
			User^user = GetSendUser(this);
			if (user->CheckLoginInfo(this->CurrentLoginInfo))
			{
			}
			else
			{
				throw gcnew InvalidOperationException();
			}
		}

		void WenceyWang::LiveChatDemo::SendMessagePackage::Process()
		{
			ClientPackage::Process();
			User^sender = GetSendUser(this);
			User^receiver = GetUser(this->TargetUser);
			lock l(receiver);
			receiver->Messages->Enqueue(gcnew MessagePackage(sender->Guid, this->Content, (Server::App::Current)->Port));
			l.release();
		}

		void WenceyWang::LiveChatDemo::GetMessagesPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSendUser(this);
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
			User^user = GetSendUser(this);
			ReturnUsersPackage^ package = gcnew ReturnUsersPackage(Enumerable::ToList(Enumerable::Select(Enumerable::Select(user->Friends, gcnew Func<Guid, int, User^>(GetUserWithIndex)), gcnew Func<User^, int, UserInfo^>(User::ToUserInfo))), this->Source);
			(Server::App::Current)->SendPackage(package);


		}

		void WenceyWang::LiveChatDemo::AddFriendPackage::Process()
		{
			ClientPackage::Process();
			User^user = GetSendUser(this);
			User^Friend = GetUser(this->TargetUser);
			user->Friends->Add(*Friend->Guid);
		}

		void WenceyWang::LiveChatDemo::RegisAccountPackage::Process()
		{
			//No logininfo check
			Server::App::Current->Users->Add(gcnew User(Guid::NewGuid(), this->Name, this->Password));
		}
	}
}


int main(array<System::String ^> ^args)
{
	WenceyWang::LiveChatDemo::Server::App^ App = gcnew WenceyWang::LiveChatDemo::Server::App();

	App->Start();

	return 0;
}

