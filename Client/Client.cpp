// Client.cpp : main project file.

#include "stdafx.h"

using namespace System;

//I should use FoggyConsole To drow UI But the Time is not enough ┑(￣Д ￣)┍

namespace WenceyWang {

	namespace LiveChatDemo
	{
		namespace Client {
			
			
			public ref class App
			{
			public:

				App()
				{

				}

				void Start()
				{

				}

			};
		}
	}
}


int main(array<System::String ^> ^args)
{
	WenceyWang::LiveChatDemo::Client::App^ App = gcnew WenceyWang::LiveChatDemo::Client::App();

	App->Start();

	return 0;
}
