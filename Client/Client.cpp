// Client.cpp : main project file.

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

//I should use FoggyConsole To drow UI But the Time is not enough ┑(￣Д ￣)┍

namespace WenceyWang {

	namespace LiveChatDemo
	{
		namespace Client {
			

			public ref class App:Application
			{
				IPEndPoint^ Server;
				Thread^ ListenThread;
				UdpClient^ Listener;
				LoginInfo^ UserLoginInfo;

			public:

				App()
				{

				}

				void GetMessage()
				{
					LogInfo("Bind to Port {0}", Server->Port);
					Listener = gcnew UdpClient(Server);
					LogInfo("Bind Success");

					LogInfo("Search Package Types");
					array<Type^>^ types = Array::FindAll(this->GetType()->Assembly->GetTypes(), gcnew Predicate<Type^>(this, &App::ChooseServerPackageType));

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
					ListenThread = gcnew Thread(gcnew ThreadStart(this, &App::GetMessage));
					ListenThread->Start();
					LogInfo("Lisiten Thread Started");


				}

			};
		}
	}
}


int main(array<System::String ^> ^args)
{
	WenceyWang::LiveChatDemo::Client::App^ App = gcnew WenceyWang::LiveChatDemo::Client::App();

	App->Start();

	return 0;
}
