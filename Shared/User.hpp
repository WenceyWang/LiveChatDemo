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

#include"InterOp.hpp"
#include"ServerPackage.hpp"
#include"LoginInfo.hpp"

namespace WenceyWang {

	namespace LiveChatDemo
	{
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

			bool CheckLoginInfo(LoginInfo^ loginInfo)
			{
				return Name == loginInfo->Name
					&&
					Convert::ToBase64String(this->PasswordHashed) == Convert::ToBase64String(Hash->ComputeHash(InterOp::ToByte(loginInfo->Password)));
			}


		};
	}
}
