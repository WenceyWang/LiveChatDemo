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

				ConsoleLocker = gcnew Object();

			void LogInfo(String^ formart, ... array<System::Object^>^ arg)
			{
				lock l(ConsoleLocker);

				Console::BackgroundColor = ConsoleColor::DarkMagenta;
				Console::ForegroundColor = ConsoleColor::White;

				Console::Write("[INFO]");

				Console::ResetColor();
				Console::Write("	");

				Console::ForegroundColor = ConsoleColor::White;

				Console::Write(DateTime::UtcNow);
				Console::Write("	");
				Console::WriteLine(formart, arg);
				Console::ResetColor();

				l.release();
			}

			void LogDebug(String^ formart, ... array<System::Object^>^ arg)
			{
				lock l(ConsoleLocker);

				Console::BackgroundColor = ConsoleColor::DarkBlue;
				Console::ForegroundColor = ConsoleColor::White;

				Console::Write("[DEBUG]");

				Console::ResetColor();
				Console::Write("	");

				Console::ForegroundColor = ConsoleColor::White;

				Console::Write(DateTime::UtcNow);
				Console::Write("	");
				Console::WriteLine(formart, arg);
				
				Console::ResetColor();

				l.release();
			}

			void LogWarn(String^ formart, ... array<System::Object^>^ arg)
			{
				lock l(ConsoleLocker);
				Console::BackgroundColor = ConsoleColor::DarkRed;				
				Console::ForegroundColor = ConsoleColor::White;

				Console::Write("[WARNING]");
				
				Console::ResetColor();
				Console::Write("	");

				Console::ForegroundColor = ConsoleColor::White;
				
				Console::Write(DateTime::UtcNow);
				Console::Write("	");
				Console::WriteLine(formart, arg);
				
				Console::ResetColor();

				l.release();

			}

			virtual void Start() abstract;


		};
	}
}
