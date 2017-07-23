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
		public ref class InterOp abstract sealed {

		public:
			static XElement^ ToXElement(array<unsigned char>^ data)
			{
				String^ string = Encoding::UTF8->GetString(data);

				return XElement::Parse(string);
			}

			static array<unsigned char>^ ToByte(String^ value)
			{
				return Encoding::UTF8->GetBytes(value);
			}

#pragma region Copy From https://stackoverflow.com/questions/2727609/best-way-to-create-ipendpoint-from-string

			static int getPort(String^ p)
			{
				int port;

				if (!int::TryParse(p, port)
					|| port < IPEndPoint::MinPort
					|| port > IPEndPoint::MaxPort)
				{
					throw gcnew FormatException(String::Format("Invalid end point port '{0}'", p));
				}

				return port;
			}

			static IPAddress^ getIPfromHost(String^ p)
			{
				array<IPAddress^>^ hosts = Dns::GetHostAddresses(p);

				if (hosts == nullptr || hosts->Length == 0)
					throw gcnew ArgumentException(String::Format("Host not found: {0}", p));

				return hosts[0];
			}


			static IPEndPoint^ Parse(String^ endpointstring, int defaultport)
			{
				if (String::IsNullOrEmpty(endpointstring)
					|| endpointstring->Trim()->Length == 0)
				{
					throw gcnew ArgumentException("Endpoint descriptor may not be empty.");
				}

				if (defaultport != -1 &&
					(defaultport < IPEndPoint::MinPort
						|| defaultport > IPEndPoint::MaxPort))
				{
					throw gcnew ArgumentException(String::Format("Invalid default port '{0}'", defaultport));
				}

				array<String^>^ values = endpointstring->Split(gcnew array<Char> { ':' });
				IPAddress^ ipaddy;
				int port = -1;

				//check if we have an IPv6 or ports
				if (values->Length <= 2) // ipv4 or hostname
				{
					if (values->Length == 1)
						//no port is specified, default
						port = defaultport;
					else
						port = getPort(values[1]);

					//try to use the address as IPv4, otherwise get hostname
					if (!IPAddress::TryParse(values[0], ipaddy))
						ipaddy = getIPfromHost(values[0]);
				}
				else if (values->Length > 2) //ipv6
				{
					//could [a:b:c]:d
					if (values[0]->StartsWith("[") && values[((values->Length) - 2)]->EndsWith("]"))
					{
						String^ ipaddressstring = String::Join(":", Enumerable::ToArray(Enumerable::Take<String^>(values, values->Length - 1)));
						ipaddy = IPAddress::Parse(ipaddressstring);
						port = getPort(values[values->Length - 1]);
					}
					else //[a:b:c] or a:b:c
					{
						ipaddy = IPAddress::Parse(endpointstring);
						port = defaultport;
					}
				}
				else
				{
					throw gcnew FormatException(String::Format("Invalid endpoint ipaddress '{0}'", endpointstring));
				}

				if (port == -1)
					throw gcnew ArgumentException(String::Format("No port specified: '{0}'", endpointstring));

				return gcnew IPEndPoint(ipaddy, port);
			}



			static IPEndPoint^ Parse(String^ endpointstring)
			{
				return Parse(endpointstring, 3344);
			}


#pragma endregion

		};
	}
}
