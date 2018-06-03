using System;
using System.Collections.Generic;
using System . IO ;
using System.Linq;
using System . Net ;
using System . Net . Sockets ;
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

namespace Server
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
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
				StreamWriter writer = new StreamWriter(Client.GetStream());

				writer.WriteLine(MessageBox.Text);

				writer.Flush();
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
					MessageList.Items.Add($"Client:{await streamReader.ReadLineAsync()}");
				}

			}
			catch (Exception exception)
			{
				MessageList.Items.Add(exception.ToString());
			}
		}

		private async void Listen()
		{
			try
			{
				TcpListener listener = new TcpListener(IPAddress.Any,int.Parse(PortTextBox.Text));

				listener . Start ( ) ;

				Client = await listener . AcceptTcpClientAsync ( ) ;

				Dispatcher . Invoke ( GetMessage ) ;

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
				Dispatcher.Invoke(Listen);
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
