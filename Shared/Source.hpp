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

			virtual XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("Name", Name);
				element->SetAttributeValue("Password", Password);

				return element;
			}

			virtual void Process() override;

		};

		public ref class SendMessagePackage :ClientPackage
		{
		public:

			String^ TargetUser;

			String^ Content;

			SendMessagePackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				TargetUser = element->Attribute("TargetUser")->Value;
				Content = element->Attribute("Content")->Value;
			}

			SendMessagePackage(String^ targetUser, String^ content, IPAddress^target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetUser = targetUser;
				Content = content;
			}

			virtual XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("TargetUser", TargetUser);
				element->SetAttributeValue("Content", Content);

				return element;
			}

			virtual void Process() override;

		};

		public ref class SendGroupMessagePackage :ClientPackage
		{
		public:

			String^ TargetGroup;

			String^ Content;

			SendGroupMessagePackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				TargetGroup = element->Attribute("TargetGroup")->Value;
				Content = element->Attribute("Content")->Value;
			}

			SendGroupMessagePackage(String^ targetGroup, String^ content, IPAddress^target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetGroup = targetGroup;
				Content = content;
			}

			virtual XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("TargetGroup", TargetGroup);
				element->SetAttributeValue("Content", Content);

				return element;
			}

		virtual	void Process() override;

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

		virtual	void Process() override;

		};

		public ref class AddFriendPackage :ClientPackage
		{
		public:

			String^ TargetUser;

			AddFriendPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				TargetUser = element->Attribute("TargetUser")->Value;


			}

			AddFriendPackage(String^ targetUser, IPAddress^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetUser = targetUser;
			}

			virtual XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("TargetUser", TargetUser);

				return element;
			}

		virtual	void Process() override;

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

		virtual	void Process() override;

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

		virtual	void Process() override;

		};

		public ref class GetGroupUserPackage:ClientPackage
		{
		public:
			String^ TargetGroup;


			GetGroupUserPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				TargetGroup = element->Attribute("TargetGroup")->Value;
			}

			GetGroupUserPackage(String^ targetGroup, IPAddress^target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetGroup = targetGroup;
			}

			virtual XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("TargetGroup", TargetGroup);

				return element;
			}

		virtual	void Process() override;

		};

		public ref class ReturnUsersPackage :ServerPackage
		{

		public:
			List<UserInfo^>^ Users;

			ReturnUsersPackage(List<UserInfo^>^ users, IPAddress ^ target) :ServerPackage(target)
			{
				Users = users;
			}

			virtual String^ ToString()override
			{
				StringBuilder^ builder = gcnew StringBuilder();

				for each (UserInfo^ user in Users)
				{
					builder->AppendLine(user->ToString());
				}

				return builder->ToString();
			}

			ReturnUsersPackage(IPAddress^ source, XElement^ element) :ServerPackage(source, element)
			{
				Users = gcnew List<UserInfo^>();

				for each (XElement^ user in element->Elements())
				{
					Users->Add(gcnew UserInfo(user));
				}

			}

			virtual XElement^ ToXElement() override
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


			ReturnFriendsPackage(List<UserInfo^>^ users, IPAddress ^ target) :ServerPackage(target)
			{
				Users = users;
			}

			ReturnFriendsPackage(IPAddress^ source, XElement^ element) :ServerPackage(source, element)
			{
				Users = gcnew List<UserInfo^>();

				for each (XElement^ user in element->Elements())
				{
					Users->Add(gcnew UserInfo(user));
				}

			}

		virtual	String^ ToString()override
			{
				StringBuilder^ builder = gcnew StringBuilder();

				for each (UserInfo^ user in Users)
				{
					builder->AppendLine(user->ToString());
				}

				return builder->ToString();
			}

		virtual	XElement^ ToXElement() override
			{
				XElement^ element = ServerPackage::ToXElement();

				for each (UserInfo^ user in Users)
				{
					element->Add(user->ToXElement());

				}
				return element;
			}
		};

		public ref class ReturnGroupUsersPackage :ServerPackage
		{
		public:

			List<UserInfo^>^ Users;


			ReturnGroupUsersPackage(List<UserInfo^>^ users, IPAddress ^ target) :ServerPackage(target)
			{
				Users = users;
			}

			ReturnGroupUsersPackage(IPAddress^ source, XElement^ element) :ServerPackage(source, element)
			{
				Users = gcnew List<UserInfo^>();

				for each (XElement^ user in element->Elements())
				{
					Users->Add(gcnew UserInfo(user));
				}

			}

			virtual String^ ToString()override
			{
				StringBuilder^ builder = gcnew StringBuilder();

				for each (UserInfo^ user in Users)
				{
					builder->AppendLine(user->ToString());
				}

				return builder->ToString();
			}

			virtual XElement^ ToXElement() override
			{
				XElement^ element = ServerPackage::ToXElement();

				for each (UserInfo^ user in Users)
				{
					element->Add(user->ToXElement());

				}
				return element;
			}
		};


		public ref class BlockPeoplePackage:ClientPackage
		{
		public:

			String^ TargetUser;

			BlockPeoplePackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{

				TargetUser = element->Attribute("TargetUser")->Value;

			}


			virtual XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("TargetUser", TargetUser);

				return element;
			}

			BlockPeoplePackage(String^ targetUser, IPAddress^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetUser = targetUser;
			}

			virtual void Process() override;

		};

		public ref class MessagePackage :ServerPackage
		{
		public:

			String^ SourceUser;

			String^ Content;

			MessagePackage(IPAddress^ source, XElement^ element) :ServerPackage(source, element)
			{
				SourceUser = element->Attribute("SourceUser")->Value;
				Content = element->Attribute("Content")->Value;
			}

			MessagePackage(String^ sourceUser, String^ content) :ServerPackage()
			{
				SourceUser = sourceUser;
				Content = content;
			}

			virtual String^ ToString()override
			{
				return String::Format("{0}	{1}:{2}",DateTime::UtcNow, SourceUser, Content);
			}

		virtual	XElement^ ToXElement()override
			{
				XElement^ element = ServerPackage::ToXElement();
				element->SetAttributeValue("SourceUser", SourceUser);
				element->SetAttributeValue("Content", Content);
				return element;
			}

		};

		public ref class CreateGroupPackage:ClientPackage
		{
		public:
			String^ Name;

			CreateGroupPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				Name = element->Attribute("Name")->Value;
			}

			CreateGroupPackage(String^ name,  IPAddress^target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				Name = name;
			}

			virtual XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("Name", Name);

				return element;
			}

		virtual	void Process() override;


		};

		public ref class GroupAddUserPackage :ClientPackage
		{
		public:

			String^ TargetUser;

			String^ TargetGroup;

			GroupAddUserPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				TargetUser = element->Attribute("TargetUser")->Value;
				TargetGroup = element->Attribute("TargetGroup")->Value;
			}

			GroupAddUserPackage(String^ targetUser, String^ targetGroup, IPAddress^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetUser = targetUser;
				TargetGroup = targetGroup;
			}

			virtual XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("TargetUser", TargetUser);
				element->SetAttributeValue("TargetGroup", TargetGroup);

				return element;
			}

		virtual	void Process() override;

		};


		public ref class GroupRemoveUserPackage :ClientPackage
		{
		public:

			String^ TargetUser;

			String^ TargetGroup;

			GroupRemoveUserPackage(IPAddress^ source, XElement^ element) :ClientPackage(source, element)
			{
				TargetUser = element->Attribute("TargetUser")->Value;
				TargetGroup = element->Attribute("TargetGroup")->Value;
			}

			GroupRemoveUserPackage(String^ targetUser, String^ targetGroup, IPAddress^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetUser = targetUser;
				TargetGroup = targetGroup;
			}

			virtual XElement^ ToXElement()override
			{
				XElement^ element = ClientPackage::ToXElement();

				element->SetAttributeValue("TargetUser", TargetUser);
				element->SetAttributeValue("TargetGroup", TargetGroup);

				return element;
			}

		virtual	void Process() override;

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

		public ref class GroupNamePredicate
		{
		public:

			String^ Name;

			GroupNamePredicate(String^ name)
			{
				Name = name;
			}

			bool ChooseName(Group^ group)
			{
				return group->Name == this->Name;
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
