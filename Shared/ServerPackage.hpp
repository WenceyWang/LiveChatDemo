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
using namespace System::Security::Cryptography;
using namespace msclr;

#include"Package.hpp"

namespace WenceyWang {

	namespace LiveChatDemo
	{

		public ref class ServerPackage abstract :Package
		{
		public:

			static bool ChooseServerPackageType(Type^ type)
			{
				Type ^t = ServerPackage::typeid;
				return t->IsAssignableFrom(type) && (!type->IsAbstract);
			}


			ServerPackage(IPEndPoint^source, XElement^ element) :Package(source, gcnew IPEndPoint(IPAddress::Loopback, source->Port), element)
			{

			}

			ServerPackage(IPEndPoint^ target) :Package(gcnew IPEndPoint(IPAddress::Loopback, target->Port), target, nullptr)
			{

			}

			ServerPackage(int port) :Package(gcnew IPEndPoint(IPAddress::Loopback,port ),nullptr, nullptr)
			{

			}



		};
	}
}
