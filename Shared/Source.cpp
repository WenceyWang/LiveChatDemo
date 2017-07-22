#include <msclr\lock.h>

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

#pragma region Copy From https://stackoverflow.com/questions/2727609/best-way-to-create-ipendpoint-from-string

			static int getPort(String^ p)
			{
				int port;

				if (!int::TryParse(p, port)
					|| port < IPEndPoint::MinPort
					|| port > IPEndPoint::MaxPort)
				{
					throw gcnew FormatException(String::Format("Invalid end point port '{0}'", p));
				}

				return port;
			}

			static IPAddress^ getIPfromHost(String^ p)
			{
				array<IPAddress^>^ hosts = Dns::GetHostAddresses(p);

				if (hosts == nullptr || hosts->Length == 0)
					throw gcnew ArgumentException(String::Format("Host not found: {0}", p));

				return hosts[0];
			}


			static IPEndPoint^ Parse(String^ endpointstring, int defaultport)
			{
				if (String::IsNullOrEmpty(endpointstring)
					|| endpointstring->Trim()->Length == 0)
				{
					throw gcnew ArgumentException("Endpoint descriptor may not be empty.");
				}

				if (defaultport != -1 &&
					(defaultport < IPEndPoint::MinPort
						|| defaultport > IPEndPoint::MaxPort))
				{
					throw gcnew ArgumentException(String::Format("Invalid default port '{0}'", defaultport));
				}

				array<String^>^ values = endpointstring->Split(gcnew array<Char> { ':' });
				IPAddress^ ipaddy;
				int port = -1;

				//check if we have an IPv6 or ports
				if (values->Length <= 2) // ipv4 or hostname
				{
					if (values->Length == 1)
						//no port is specified, default
						port = defaultport;
					else
						port = getPort(values[1]);

					//try to use the address as IPv4, otherwise get hostname
					if (!IPAddress::TryParse(values[0], ipaddy))
						ipaddy = getIPfromHost(values[0]);
				}
				else if (values->Length > 2) //ipv6
				{
					//could [a:b:c]:d
					if (values[0]->StartsWith("[") && values[((values->Length) - 2)]->EndsWith("]"))
					{
						String^ ipaddressstring = String::Join(":", Enumerable::ToArray(Enumerable::Take<String^>(values, values->Length - 1)));
						ipaddy = IPAddress::Parse(ipaddressstring);
						port = getPort(values[values->Length - 1]);
					}
					else //[a:b:c] or a:b:c
					{
						ipaddy = IPAddress::Parse(endpointstring);
						port = defaultport;
					}
				}
				else
				{
					throw gcnew FormatException(String::Format("Invalid endpoint ipaddress '{0}'", endpointstring));
				}

				if (port == -1)
					throw gcnew ArgumentException(String::Format("No port specified: '{0}'", endpointstring));

				return gcnew IPEndPoint(ipaddy, port);
			}



			static IPEndPoint^ Parse(String^ endpointstring)
			{
				return Parse(endpointstring, 3344);
			}


#pragma endregion

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

			LoginInfo(String^ name,String^ password)
			{
				Name = name;
				Password = password;
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

			ClientPackage(IPAddress^ source, XElement^ element) :Package(source, IPAddress::Loopback,element)
			{
				this->CurrentLoginInfo = gcnew LoginInfo(element->Element("LoginInfo"));
			}

			ClientPackage(IPAddress^ target,LoginInfo^ loginInfo):Package(IPAddress::Loopback,target ,nullptr)
			{
				CurrentLoginInfo = loginInfo;
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
				Target = System::Guid::Parse(element->Attribute("Target")->Value);
				Content= element->Attribute("Content")->Value;
			}


		};

		public ref class GetMessagesPackage:ClientPackage
		{
		public:

			GetMessagesPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

		};

		public ref class MessagePackage:ServerPackage
		{
		public:
			MessagePackage()


		};


		public ref class ServerPackage abstract :Package
		{
		public:

			virtual XElement^ ToXElement()
			{
				Type^ type = this->GetType();

				XElement^ result = gcnew XElement((type->Name));

				return result;
			}

			ServerPackage(IPAddress^ target, XElement^ element) :Package(IPAddress::Loopback, target, element)
			{

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

			Queue<ServerPackage^>^  Messages = gcnew Queue<ServerPackage^>();//Todo:序列化這個

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

		public ref class Application abstract
		{
		public:

			bool ChooseClientPackageType(Type^ type)
			{
				Type ^t = ClientPackage::typeid;
				return t->IsAssignableFrom(type) && (!t->IsAbstract);
			}

			bool ChooseServerPackageType(Type^ type)
			{
				Type ^t = ServerPackage::typeid;
				return t->IsAssignableFrom(type) && (!t->IsAbstract);
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

			virtual void Start() abstract;


		};

	}
}
