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
				return t->IsAssignableFrom(type) && (!type->IsAbstract);
			}

			LoginInfo^ CurrentLoginInfo;

			ClientPackage(IPEndPoint^ source, XElement^ element) :Package(source,gcnew IPEndPoint(IPAddress::Loopback, source->Port), element)
			{
				this->CurrentLoginInfo = gcnew LoginInfo(element->Element("LoginInfo"));
			}

			ClientPackage(IPEndPoint^ target, LoginInfo^ loginInfo) :Package(( gcnew IPEndPoint(IPAddress::Loopback, target->Port)), target, nullptr)
			{
				CurrentLoginInfo = loginInfo;
			}

			virtual void Process();

		};
	}
}
