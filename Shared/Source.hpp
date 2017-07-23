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

		public ref class SendMessagePackage :ClientPackage
		{
		public:

			Guid^ TargetUser;

			String^ Content;

			SendMessagePackage(IPEndPoint^ source, XElement^ element) :ClientPackage(source, element)
			{
				TargetUser = System::Guid::Parse(element->Attribute("TargetUser")->Value);
				Content = element->Attribute("Content")->Value;
			}

			SendMessagePackage(Guid^ targetUser, String^ content, IPEndPoint^target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetUser = targetUser;
				Content = content;
			}

			XElement^ ToXElement()override
			{
				XElement^ element = Package::ToXElement();


				return element;
			}

			void Process() override;

		};

		public ref class GetMessagesPackage :ClientPackage
		{
		public:

			GetMessagesPackage(IPEndPoint^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

			GetMessagesPackage(IPEndPoint^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{

			}

			void Process() override;

		};

		public ref class AddFriendPackage :ClientPackage
		{
		public:

			Guid^ TargetUser;

			AddFriendPackage(IPEndPoint^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

			AddFriendPackage(Guid targetUser,IPEndPoint^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				TargetUser = targetUser;
			}

			void Process() override;

		};


		public ref class GetUsersPackage :ClientPackage
		{
		public:
			GetUsersPackage(IPEndPoint^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

			GetUsersPackage(IPEndPoint^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{
				
			}

			void Process() override;

		};

		public ref class GetFriendsPackage :ClientPackage
		{
		public:
			GetFriendsPackage(IPEndPoint^ source, XElement^ element) :ClientPackage(source, element)
			{

			}

			GetFriendsPackage(IPEndPoint^ target, LoginInfo^ loginInfo) :ClientPackage(target, loginInfo)
			{

			}

			void Process() override;

		};



		public ref class ReturnUsersPackage :ServerPackage
		{

		public:
			List<UserInfo^>^ Users;

			ReturnUsersPackage(List<UserInfo^>^ users, IPEndPoint ^ target) :ServerPackage(target)
			{
				Users = users;
			}

			ReturnUsersPackage(IPEndPoint^ source, XElement^ element) :ServerPackage(source, element)
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

			ReturnFriendsPackage(IPEndPoint^ source, XElement^ element) :ServerPackage(source, element)
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

			MessagePackage(IPEndPoint^ source, XElement^ element) :ServerPackage(source, element)
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
				Name == name;
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
				Name == name;
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
