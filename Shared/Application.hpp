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
		public ref class Application abstract
		{
		public:

			Object^ ConsoleLocker = gcnew Object();

			void LogInfo(String^ formart, ... array<System::Object^>^ arg)
			{
				lock l(ConsoleLocker);
				Console::Write("[INFO]	");
				Console::Write(DateTime::UtcNow);
				Console::Write("	");
				Console::WriteLine(formart, arg);
				l.release();
			}

			void LogDebug(String^ formart, ... array<System::Object^>^ arg)
			{
				lock l(ConsoleLocker);
				Console::Write("[DEBUG]");
				Console::Write(DateTime::UtcNow);
				Console::Write("	");
				Console::WriteLine(formart, arg);
				l.release();
			}

			virtual void Start() abstract;


		};
	}
}
