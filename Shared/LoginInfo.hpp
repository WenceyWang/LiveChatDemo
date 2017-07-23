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
		public ref class LoginInfo
		{
		public:
			String^ Name;
			String ^Password;

			LoginInfo(XElement^ element)
			{
				this->Name = element->Attribute("Name")->Value;
				this->Password = element->Attribute("Password")->Value;
			}

			LoginInfo(String^ name, String^ password)
			{
				Name = name;
				Password = password;
			}

			XElement^ ToXElement() override
			{
				Type^ type = this->GetType();

				XElement^ element = gcnew XElement((type->Name));

				element->SetAttributeValue("Name", Name);
				element->SetAttributeValue("Password", Password);

				return element;

			}

		};
	}
}