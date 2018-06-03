using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml.Linq;

namespace Client
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		public static IPEndPoint CreateIPEndPoint(string endPoint)
		{
			string[] ep = endPoint.Split(':');
			if (ep.Length < 2)
			{
				throw new FormatException("Invalid endpoint format");
			}

			IPAddress ip;
			if (ep.Length > 2)
			{
				if (!IPAddress.TryParse(string.Join(":", ep, 0, ep.Length - 1), out ip))
				{
					throw new FormatException("Invalid ip-adress");
				}
			}
			else
			{
				if (!IPAddress.TryParse(ep[0], out ip))
				{
					throw new FormatException("Invalid ip-adress");
				}
			}

			if (!int.TryParse(ep[ep.Length - 1], NumberStyles.None, NumberFormatInfo.CurrentInfo, out int port))
			{
				throw new FormatException("Invalid port");
			}
			return new IPEndPoint(ip, port);
		}

		public MainWindow()
		{
			InitializeComponent();
			Loaded += MainWindow_Loaded;
			ConnectButton.Click += ConnectButton_Click;
			SendButton.Click += SendButton_Click;
		}

		private TcpClient Client { get; set; }


		private void SendButton_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				StreamWriter writer = new StreamWriter ( Client . GetStream ( ) ) ;

				writer . WriteLine ( MessageBox . Text ) ;

				writer . Flush ( ) ;
			}
			catch (Exception exception)
			{
				MessageList.Items.Add(exception.ToString());
			}
		}

		private async void GetMessage()
		{
			try
			{
				StreamReader streamReader = new StreamReader(Client.GetStream());

				while (true)
				{
					MessageList.Items.Add($"Server:{await streamReader.ReadLineAsync()}");
				}

			}
			catch (Exception exception)
			{
				MessageList.Items.Add(exception.ToString());
			}
		}


		private void ConnectButton_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				IPEndPoint endPoint = CreateIPEndPoint(ServerTextBox.Text);
				Client = new TcpClient();
				Client.Connect(endPoint);
				Dispatcher.Invoke(GetMessage);
			}
			catch (Exception exception)
			{
				MessageList.Items.Add(exception.ToString());
			}
		}

		private void MainWindow_Loaded(object sender, RoutedEventArgs e)
		{
		}
	}
}
