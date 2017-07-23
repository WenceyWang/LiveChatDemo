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

#include"ClientPackage.hpp"
#include"ServerPackage.hpp"
#include"Application.hpp"
#include"User.hpp"

namespace WenceyWang {

	namespace LiveChatDemo
	{

		public ref class SendMessagePackage :ClientPackage
		{
		public:

			Guid^ Target;

			String^ Content;

			SendMessagePackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				Target = System::Guid::Parse(element->Attribute("Target")->Value);
				Content = element->Attribute("Content")->Value;
			}


		};

		public ref class GetMessagesPackage :ClientPackage
		{
		public:

			GetMessagesPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

		};

		public ref class GetUsersPackage:ClientPackage
		{
		public:
			GetUsersPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{

			}
		};


		public ref class ReturnUsersPackage :ServerPackage
		{

		public:
			ReturnUsersPackage(IPAddress^ source, XElement^ element) :ServerPackage(source, element)
			{

			}



		};


			public ref class MessagePackage :ServerPackage
		{
		public:
			MessagePackage(IPAddress^ target):ServerPackage(target)
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


	}
}
