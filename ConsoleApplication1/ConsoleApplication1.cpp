// ConsoleApplication1.cpp : main project file.

#include "stdafx.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Xml::Linq;
//using namespace System::Linq;
using namespace System::Text;
using namespace  System::Security::Cryptography;
using namespace msclr;

namespace WenceyWang {

	namespace LiveChatDemo
	{
		namespace Server {

			public ref class InterOp abstract sealed {

			public:
				static XElement^ ToXElement(array<unsigned char>^ data)
				{
					String^ string = Encoding::UTF8->GetString(data);

					return XElement::Parse(string);
				}

				static array<unsigned char>^ ToByte(String^ value)
				{
					return Encoding::UTF8->GetBytes(value);
				}
			};


			public ref class User sealed
			{

			public:

				Guid ^ Guid;
				String ^ Name;

				//Todo: Salt the Hash
				array<unsigned char>^ PasswordHashed;

				static SHA512Managed^ Hash = gcnew SHA512Managed();

				List<System::Guid>^ Friends = gcnew List<System::Guid>();

				User(System::Guid^ guid, String^ name, String^ password)
				{
					this->Guid = guid;
					this->Name = name;
					this->PasswordHashed = Hash->ComputeHash(InterOp::ToByte(password));
				}

				void AddFriend(System::Guid guid)
				{
					Friends->Add(guid);
				}

				User(XElement^ element)
				{
					this->Guid = System::Guid::Parse(element->Attribute("Guid")->Value);
					this->Name = element->Attribute("Name")->Value;
					this->PasswordHashed = Convert::FromBase64String(element->Attribute("Password")->Value);

					for each (XElement^ var in element->Element("Friends")->Elements())
					{
						Friends->Add(System::Guid::Parse(var->Value));
					}

				}

				XElement^ ToXelement()
				{
					XElement^ element = gcnew XElement("User");

					element->SetAttributeValue("Guid", this->Guid->ToString());
					element->SetAttributeValue("Name", this->Name);
					element->SetAttributeValue("Password", Convert::ToBase64String(this->PasswordHashed));

					XElement^ friendsElement = gcnew XElement("Friends");

					for each (System::Guid _friend in Friends)
					{
						XElement^ current = gcnew XElement("Friend");
						current->Add(_friend);
						friendsElement->Add(current);
					}

					element->Add(friendsElement);

					return element;

				}




			};

			public ref class LoginInfo
			{
			public:
				String^ Name;
				String ^Password;

				LoginInfo(XElement^ element)
				{
					this->Name = element->Attribute("Name")->Value;
					this->Password = element->Attribute("Name")->Value;
				}

			};

			public ref class Package abstract
			{
			public:

				IPAddress^ Source;
				IPAddress^ Target;

				Package(IPAddress^ source, IPAddress^ target, XElement^ element)
				{
					Source = source;
					Target = target;
					Element = element;
				}

			protected:

				XElement^ Element;



			};

			public ref class ClientPackage abstract :Package
			{



			public:

				LoginInfo^ CurrentLoginInfo;

				ClientPackage(IPAddress^ source, XElement^ element) :Package(source, IPAddress::Loopback, element)
				{
					this->CurrentLoginInfo = gcnew LoginInfo(element->Element("LoginInfo"));
				}

				virtual void  Process()
				{
					
				}

			};

			public ref class SendMessagePackage :ClientPackage
			{
			public:

				Guid^ Target;

				String^ Content;

				SendMessagePackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
				{

				}


			};




			public ref class ServerPackage abstract :Package
			{
			public:

				virtual XElement^ ToXElement()
				{
					Type^ type = this->GetType();

					XElement^ result = gcnew XElement((type->FullName));

					return result;
				}

				ServerPackage(IPAddress^ target, XElement^ element) :Package(IPAddress::Loopback, target, element)
				{

				}

			};



			public ref class TypeNamePredicate
			{
			public:

				String^ Name;

				TypeNamePredicate(String^ name)
				{
					Name == name;
				}

				bool ChooseName(Type^ type)
				{
					return type->Name == this->Name;
				}


			};

			public ref class App
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

					}
				}

				bool ChooseType(Type^ type)
				{
					Type ^t = ClientPackage::typeid;
					return t->IsAssignableFrom(type) && (!t->IsAbstract);
				}

				void Listen()
				{

					LogInfo("Bind to Port {0}", Port);
					Listener = gcnew UdpClient(gcnew IPEndPoint(IPAddress::Any, Port));
					LogInfo("Bind Success");

					LogInfo("Search Package Types");
					array<Type^>^ types = Array::FindAll(this->GetType()->Assembly->GetTypes(), gcnew Predicate<Type^>(this, &App::ChooseType));

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

						}
						else
						{
							Thread::Sleep(500);
						}
						l.release();
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
							XElement^ element = toSent->ToXElement();
							array<unsigned char>^ bytes = InterOp::ToByte(element->ToString());
							Sender->Send(bytes, bytes->Length, gcnew IPEndPoint(toSent->Target, Port));
						}
						else
						{
							Thread::Sleep(500);
						}
						l.release();
					}
				}

				void LogInfo(String^ formart, ... array<System::Object^>^ arg)
				{
					Console::Write("[INFO]	");
					Console::Write(DateTime::UtcNow);
					Console::Write("	");
					Console::WriteLine(formart, arg);
				}

				void LogDebug(String^ formart, ... array<System::Object^>^ arg)
				{
					Console::Write("[DEBUG]");
					Console::Write(DateTime::UtcNow);
					Console::Write("	");
					Console::WriteLine(formart, arg);
				}


			public:



				App()
				{

				}

				void Start()
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

