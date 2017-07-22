// ConsoleApplication1.cpp : main project file.

#include "Source.cpp"
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

namespace WenceyWang {

	namespace LiveChatDemo
	{
		namespace Server {


			public ref class App:Application
			{
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


				void Listen()
				{

					LogInfo("Bind to Port {0}", Port);
					Listener = gcnew UdpClient(gcnew IPEndPoint(IPAddress::Any, Port));
					LogInfo("Bind Success");

					LogInfo("Search Package Types");
					array<Type^>^ types = Array::FindAll(this->GetType()->Assembly->GetTypes(), gcnew Predicate<Type^>(this, &App::ChooseClientPackageType));

					LogInfo("Start Lisining");
					while (true)
					{
						IPEndPoint^ address = gcnew IPEndPoint(IPAddress::Any, 3344);
						array<unsigned char>^ value = Listener->Receive(address);
						XElement^ element = InterOp::ToXElement(value);

						TypeNamePredicate ^ namePred = gcnew TypeNamePredicate(element->Name->ToString());

						Type ^type = Array::Find(types, gcnew Predicate<Type^>(namePred, &TypeNamePredicate::ChooseName));

						ClientPackage^ package= (ClientPackage^)Activator::CreateInstance(type, address->Address, element);

						lock l(InMessage);

						InMessage->Enqueue(package);

						l.release();
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

				void Send()
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


			public:

				App()
				{

				}

				void Start() override
				{

					LogInfo("Server Starting");

					LogInfo("Starting Lisiten Thread");
					ListenThread = gcnew Thread(gcnew ThreadStart(this, &App::Listen));
					ListenThread->Start();
					LogInfo("Lisiten Thread Started");

					LogInfo("Starting Send Thread");
					SendThread = gcnew Thread(gcnew ThreadStart(this, &App::Send));
					SendThread->Start();
					LogInfo("Send Thread Started");

					LogInfo("Starting Process Thread");
					ProcessThread = gcnew Thread(gcnew ThreadStart(this, &App::Send));
					ProcessThread->Start();
					LogInfo("Process Thread Started");

				}

			};
		}
	}
}
int main(array<System::String ^> ^args)
{
	WenceyWang::LiveChatDemo::Server::App^ App = gcnew WenceyWang::LiveChatDemo::Server::App();

	App->Start();

	return 0;
}

