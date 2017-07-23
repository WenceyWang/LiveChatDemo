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

namespace WenceyWang {

	namespace LiveChatDemo
	{

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

			virtual XElement^ ToXElement()
			{
				Type^ type = this->GetType();

				XElement^ result = gcnew XElement((type->Name));

				return result;
			}

		protected:

			XElement^ Element;



		};
	}
}
