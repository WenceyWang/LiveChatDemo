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
#include"ClientPackage.hpp"
#include"ServerPackage.hpp"
#include"Application.hpp"
#include"User.hpp"
#include"InterOp.hpp"
#include"LoginInfo.hpp"

namespace WenceyWang {

	namespace LiveChatDemo
	{

		public ref class RegisAccountPackage :ClientPackage
		{
		public:

			String^ Name;

			String^ Password;

			RegisAccountPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				Name = element->Attribute("Name")->Value;
				Password = element->Attribute("Password")->Value;
			}

			RegisAccountPackage(String^ name, String^ password, IPAddress^target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				Name = name;
				Password = password;
			}

			XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("Name", Name);
				element->SetAttributeValue("Password", Password);

				return element;
			}

			void Process() override;

		};

		public ref class SendMessagePackage :ClientPackage
		{
		public:

			Guid^ TargetUser;

			String^ Content;

			SendMessagePackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				TargetUser = System::Guid::Parse(element->Attribute("TargetUser")->Value);
				Content = element->Attribute("Content")->Value;
			}

			SendMessagePackage(Guid^ targetUser, String^ content, IPAddress^target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetUser = targetUser;
				Content = content;
			}

			XElement^ ToXElement()override
			{
				XElement^ element = Package::ToXElement();

				element->SetAttributeValue("TargetUser", TargetUser);
				element->SetAttributeValue("Content", Content);

				return element;
			}

			void Process() override;

		};

		public ref class GetMessagesPackage :ClientPackage
		{
		public:

			GetMessagesPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

			GetMessagesPackage(IPAddress^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{

			}

			void Process() override;

		};

		public ref class AddFriendPackage :ClientPackage
		{
		public:

			Guid^ TargetUser;

			AddFriendPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

			AddFriendPackage(Guid targetUser, IPAddress^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetUser = targetUser;
			}

			void Process() override;

		};

		public ref class GetUsersPackage :ClientPackage
		{
		public:
			GetUsersPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

			GetUsersPackage(IPAddress^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{

			}

			void Process() override;

		};

		public ref class GetFriendsPackage :ClientPackage
		{
		public:
			GetFriendsPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

			GetFriendsPackage(IPAddress^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{

			}

			void Process() override;

		};



		public ref class ReturnUsersPackage :ServerPackage
		{

		public:
			List<UserInfo^>^ Users;

			ReturnUsersPackage(List<UserInfo^>^ users, IPAddress ^ target) :ServerPackage(target)
			{
				Users = users;
			}

			ReturnUsersPackage(IPAddress^ source, XElement^ element) :ServerPackage(source, element)
			{
				Users = gcnew List<UserInfo^>();

				for each (XElement^ element in element->Elements())
				{
					Users->Add(gcnew UserInfo(element));
				}

			}

			XElement^ ToXElement() override
			{
				XElement^ element = ServerPackage::ToXElement();

				for each (UserInfo^ user in Users)
				{
					element->Add(user->ToXElement());

				}
				return element;
			}

		};

		public ref class ReturnFriendsPackage :ServerPackage
		{
		public:

			List<UserInfo^>^ Users;

			String^ Content;

			ReturnFriendsPackage(int port) :ServerPackage(port)
			{

			}

			ReturnFriendsPackage(IPAddress^ source, XElement^ element) :ServerPackage(source, element)
			{
				Users = gcnew List<UserInfo^>();

				for each (XElement^ element in element->Elements())
				{
					Users->Add(gcnew UserInfo(element));
				}

			}

			XElement^ ToXElement() override
			{
				XElement^ element = ServerPackage::ToXElement();

				for each (UserInfo^ user in Users)
				{
					element->Add(user->ToXElement());

				}
				return element;
			}
		};



		public ref class MessagePackage :ServerPackage
		{
		public:

			Guid^ SourceUser;

			String^ Content;

			MessagePackage(int port) :ServerPackage(port)
			{

			}

			MessagePackage(IPAddress^ source, XElement^ element) :ServerPackage(source, element)
			{
				SourceUser = System::Guid::Parse(element->Attribute("TargetUser")->Value);
				Content = element->Attribute("Content")->Value;
			}

			MessagePackage(Guid^ sourceUser, String^ content, int port) :ServerPackage(port)
			{
				SourceUser = sourceUser;
				Content = content;
			}

			XElement^ ToXElement()override
			{
				XElement^ element = Package::ToXElement();


				return element;
			}

		};

		public ref class TypeNamePredicate
		{
		public:

			String^ Name;

			TypeNamePredicate(String^ name)
			{
				Name = name;
			}

			bool ChooseName(Type^ type)
			{
				return type->Name == this->Name;
			}

		};



		public ref class UserNamePredicate
		{
		public:

			String^ Name;

			UserNamePredicate(String^ name)
			{
				Name = name;
			}

			bool ChooseName(User^ user)
			{
				return user->Name == this->Name;
			}
		};

		public ref class UserGuidPredicate
		{
		public:

			Guid^ Guid;

			UserGuidPredicate(System::Guid^ id)
			{
				Guid = id;
			}

			bool ChooseGuid(User^ user)
			{
				return user->Guid == this->Guid;
			}


		};



	}
}
