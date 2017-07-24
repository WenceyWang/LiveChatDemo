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

#include"ServerPackage.hpp"
#include"LoginInfo.hpp"
#include"InterOp.hpp"

namespace WenceyWang {

	namespace LiveChatDemo
	{
		public ref class UserInfo sealed
		{
		public:

			Guid ^ Guid;
			String ^ Name;
			bool IsOnline;

			XElement^ ToXElement()
			{
				XElement^ element = gcnew XElement("UserInfo");

				element->SetAttributeValue("Guid", this->Guid->ToString());
				element->SetAttributeValue("Name", this->Name);
				element->SetAttributeValue("IsOnline", this->IsOnline);

				return element;
			}

			UserInfo(System::Guid^ guid, String^ name, bool isOnline)
			{
				this->Guid = guid;
				this->Name = name;
				this->IsOnline = isOnline;
			}

			UserInfo(XElement^ element)
			{
				this->Guid = System::Guid::Parse(element->Attribute("Guid")->Value);
				this->Name = element->Attribute("Name")->Value;
				this->IsOnline = Convert::ToBoolean(element->Attribute("IsOnline")->Value);
			}

			virtual String ^ ToString() override
			{
				if (IsOnline)
				{
					return String::Format("{0}	Online", Name);
				}
				else
				{
					return String::Format("{0}	Offline", Name);
				}
			}

		};



		public ref class User sealed
		{

		public:

			Guid ^ Guid;
			String ^ Name;

			//Todo: Salt the Hash
			String^ PasswordHashed;

			static SHA512Managed^ Hash = gcnew SHA512Managed();

			List<User^>^ Friends;

			List<User^>^ Blockeds;

			DateTime^LastSeen;

			Queue<ServerPackage^>^  Messages;//Todo:序列化這個

			User(System::Guid guid, String^ name, String^ password)
			{
				Friends = gcnew List<User^>();
				Blockeds = gcnew List<User^>();
				Messages = gcnew Queue<ServerPackage^>();

				this->Guid = guid;
				this->Name = name;
				this->PasswordHashed = Convert::ToBase64String(Hash->ComputeHash(InterOp::ToByte(password)));
			}

			bool IsOnline()
			{
				return DateTime::operator-(DateTime::UtcNow, *LastSeen) < TimeSpan::FromSeconds(20);
			}

			User(XElement^ element)
			{
				Friends = gcnew List<User^>();
				Blockeds = gcnew List<User^>();
				Messages = gcnew Queue<ServerPackage^>();


				this->Guid = System::Guid::Parse(element->Attribute("Guid")->Value);
				this->Name = element->Attribute("Name")->Value;
				this->PasswordHashed = (element->Attribute("Password")->Value);

				/*for each (XElement^ var in element->Element("Friends")->Elements())
				{
					Friends->Add(System::Guid::Parse(var->Value));
				}

				for each (XElement^ var in element->Element("Blockeds")->Elements())
				{
					Blockeds->Add(System::Guid::Parse(var->Value));
				}*/

			}

			XElement^ ToXElement()
			{
				XElement^ element = gcnew XElement("User");

				element->SetAttributeValue("Guid", this->Guid->ToString());
				element->SetAttributeValue("Name", this->Name);
				element->SetAttributeValue("Password", this->PasswordHashed);

				XElement^ friendsElement = gcnew XElement("Friends");

				for each (User^ _friend in Friends)
				{
					XElement^ current = gcnew XElement("Friend");
					current->Add(_friend);
					friendsElement->Add(current);
				}

				element->Add(friendsElement);

				XElement^ blockedElement = gcnew XElement("Blockeds");

				for each (User^ _blocked in Blockeds)
				{
					XElement^ current = gcnew XElement("Blocked");
					current->Add(_blocked);
					blockedElement->Add(current);
				}

				element->Add(blockedElement);


				return element;
			}

			static UserInfo^ ToUserInfo(User^ user, int index)
			{
				UserInfo^ result = gcnew UserInfo(user->Guid, user->Name, user->IsOnline());
				return result;
			}

			bool CheckLoginInfo(LoginInfo^ loginInfo)
			{
				String^ passwordHash = Convert::ToBase64String(Hash->ComputeHash(InterOp::ToByte(loginInfo->Password)));
				return Name == loginInfo->Name
					&&
					this->PasswordHashed == passwordHash;
			}



		};

		public ref class Group
		{
		public:

			List<User^>^ Users;

			User^ Owner;

			String^Name;

			Group(String ^ name, User^ owner)
			{
				Users = gcnew List<User^>();

				Name = name;
				Owner = owner;
				Users->Add(Owner);
			}

		};

	}
}
