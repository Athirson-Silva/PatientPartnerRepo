using System.IO;
using System.Windows;
using System.Windows.Threading;

namespace PatientPartner
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private DispatcherTimer timer;
        private double _minBatimento = 0;
        private double _maxBatimento = 0;
        private double _minOxigenio= 0;
        private double _maxOxigenio = 0;
        public MainWindow()
        {
            InitializeComponent();
            double[] dataX = { 1, 2, 3, 4, 5 };
            double[] dataY = { 100, 94, 99, 116,105 };
            double[] data2X = { 1, 2, 3, 4, 5 };
            double[] data2Y = { 99, 97, 97, 98, 96 };
            WpfPlot1.Plot.Add.Scatter(dataX, dataY);
            WpfPlot1.Plot.Add.Scatter(data2X, data2Y);
            WpfPlot1.Refresh();

            timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(2);
            timer.Tick += Timer_Tick;
            timer.Start();
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            // Chama a função para ler o arquivo
            ReadFile();
        }

        private void ReadFile()
        {
            string filePath = @"C:\\Users\\ana\\Documents\\arquivo.json"; // Substitua pelo caminho do seu arquivo

            try
            {
                if (File.Exists(filePath))
                {
                    string fileContent = File.ReadAllText(filePath);
                    var values = fileContent.Split(',');

                    var heartRateStr = values[0].Split(':')[1].Replace('.', ',');
                    var spO2Str = values[1].Split(':')[1].Replace('.', ',');

                    updateData(heartRateStr, spO2Str);
                }
                else
                {
                    Console.WriteLine("Arquivo não encontrado.");
                }
            }
            catch (Exception ex)
            {
                // Trate possíveis exceções, como problemas de leitura/escrita
                Console.WriteLine($"Erro ao ler o arquivo: {ex.Message}");
            }
        }

        private void updateData(string heartRate, string spO2Str)
        {
            Oxigenacao.Text = spO2Str;
            Batimentos.Text = heartRate;

            double numHeart = double.Parse(heartRate);
            double numO2 = double.Parse(spO2Str);

            if (numO2 < _minOxigenio)
            {
                _minOxigenio = numO2;
                MinOxigenacao.Text = _minOxigenio.ToString("F2");
            }
            else if (numO2 > _maxOxigenio)
            {
                _maxOxigenio = numO2;
                MaxOxigenacao.Text = _maxOxigenio.ToString("F2");
            }

            if (numHeart < _minBatimento)
            {
                _minBatimento = numHeart;
                MinBatimentos.Text = _minBatimento.ToString("F2");
            }
            else if (numHeart > _maxBatimento)
            {
                _maxBatimento = numHeart;
                MaxBatimentos.Text = _maxBatimento.ToString("F2");
            }
        }

    }
}