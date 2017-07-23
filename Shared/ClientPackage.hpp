#pragma once
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

#include"Package.hpp"
#include"LoginInfo.hpp"

namespace WenceyWang {

	namespace LiveChatDemo
	{

		public ref class ClientPackage abstract :Package
		{



		public:

			static bool ChooseClientPackageType(Type^ type)
			{
				Type ^t = ClientPackage::typeid;
				return t->IsAssignableFrom(type) && (!t->IsAbstract);
			}



			LoginInfo^ CurrentLoginInfo;

			ClientPackage(IPAddress^ source, XElement^ element) :Package(source, IPAddress::Loopback, element)
			{
				this->CurrentLoginInfo = gcnew LoginInfo(element->Element("LoginInfo"));
			}

			ClientPackage(IPAddress^ target, LoginInfo^ loginInfo) :Package(IPAddress::Loopback, target, nullptr)
			{
				CurrentLoginInfo = loginInfo;
			}

			virtual void  Process()
			{

			}

		};
	}
}
