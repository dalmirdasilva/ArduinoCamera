// ArduinoCam Version 1.0 (2012-09-01) Firmware (Duemilanove Atmega168)
// Copyright 2012 richard.prinz@min.at
// See http://www.min.at/prinz/oe1rib/ArduinoCam/ for more infos
//
// This file is part of ArduinoCam
//
// ArduinoCam is free software and hardware design:
// you can redistribute the software and the hardware design and/or modify it under 
// the terms of the GNU General Public License as published by the Free Software Foundation, 
// either version 3 of the License, or (at your option) any later version.
//
// ArduinoCam is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ArduinoCam. 
// If not, see http://www.gnu.org/licenses/.

#region Usings
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;
using System.Threading;
#endregion

namespace SSTV
{
    public partial class Form1 : Form
    {
        #region Constants
        const int COLOR_BYTES = (320 * 240) * 2;
        const int BW_BYTES = (320 * 240);
        #endregion

        #region Variables
        SerialPort _port;
        DateTime now;
        byte[] buffer = new byte[COLOR_BYTES + 1024];
        #endregion

        #region Ctor / Dtor
        public Form1()
        {
            InitializeComponent();

            cmbPort.DataSource = SerialPort.GetPortNames();
            cmbPort.SelectedValueChanged += new EventHandler(cmbPort_SelectedValueChanged);
        }
        #endregion

        #region Event Handler
        #region cmbPort_SelectedValueChanged
        void cmbPort_SelectedValueChanged(object sender, EventArgs e)
        {
            ClosePort();
        }
        #endregion

        #region btnReset_Click
        private void btnReset_Click(object sender, EventArgs e)
        {
            OpenPort();
            if (_port == null)
                return;

            txtConsole.Text = "Reset:\r\n";
            _port.DiscardInBuffer();
            _port.Write("i");
            Thread.Sleep(300);
            txtConsole.Text += _port.ReadTo(">");
            _port.DiscardInBuffer();
        }
        #endregion
        #region btnStatus_Click
        private void btnStatus_Click(object sender, EventArgs e)
        {
            OpenPort();
            if (_port == null)
                return;

            txtConsole.Text = "Status:\r\n";
            _port.DiscardInBuffer();
            _port.Write("s");
            Thread.Sleep(300);
            txtConsole.Text += _port.ReadTo(">");
            _port.DiscardInBuffer();
        }
        #endregion
        #region btnCapture_Click
        private void btnCapture_Click(object sender, EventArgs e)
        {
            OpenPort();
            if (_port == null)
                return;

            txtConsole.Text = "Capture:\r\n";
            _port.DiscardInBuffer();
            _port.Write("x");
            Thread.Sleep(300);
            txtConsole.Text += _port.ReadTo(">");
            _port.DiscardInBuffer();
        }
        #endregion
        #region btnTransfer_Click
        private void btnTransfer_Click(object sender, EventArgs e)
        {
            OpenPort();
            if (_port == null)
                return;

            int counter = 0;
            //int wanted = (chkColor.Checked ? COLOR_BYTES : BW_BYTES);
            int wanted = COLOR_BYTES;
            int check = wanted;
            string w;

            txtConsole.Text = "Transfer:\r\n";
            progressBar1.Visible = true;
            progressBar1.Minimum = 0;
            progressBar1.Maximum = wanted;

            StartTiming();
            SetStatus("Transfering ... ");
            _port.DiscardInBuffer();

            //// bitmap
            //if (chkFormat.Checked)
            //    _port.Write(String.Format("{0}\n",
            //    (chkColor.Checked ? "B" : "b")));
            //// raw
            //else
            //    _port.Write(String.Format("{0}\n",
            //    (chkColor.Checked ? "R" : "r")));
            //Thread.Sleep(300);

            _port.Write("D");
            w = _port.ReadTo("@\r\n");

            while (check > 0)
            {
                counter += _port.Read(buffer, counter, check);
                check = wanted - counter;
                progressBar1.Value = counter;
                this.Update();
                SetStatus("Transfering ... " + counter.ToString());
            }

            w = _port.ReadTo(">");
            txtConsole.Text += w;

            _port.DiscardInBuffer();
            SetStatus("Transfering ... Done", true);
            progressBar1.Visible = false;

            SetBitmap();
        }
        #endregion
        #endregion

        #region Private Methods
        #region StartTiming
        private void StartTiming()
        {
            now = DateTime.Now;
        }
        #endregion
        #region SetStatus
        private void SetStatus(string Text)
        {
            SetStatus(Text, false);
        }

        private void SetStatus(string Text, bool ShowTiming)
        {
            lblStatus.Text = String.Format("{0}{1}",
                Text,
                (ShowTiming ? "; " + (DateTime.Now - now).TotalSeconds + " sec" : ""));
        }
        #endregion
        #region SetBitmap
        private void SetBitmap()
        {
            MemoryStream m = new MemoryStream(buffer);
            m.Position = 0;

            Bitmap b = new Bitmap(PictureBox.Width, PictureBox.Height);
            int U = 0, Y = 0, V = 0, Y2 = 0;
            int col = 0, row = 0;

            byte R = 0, G = 0, B = 0;


            U = m.ReadByte();
            Y = m.ReadByte();
            V = m.ReadByte();
            Y2 = m.ReadByte();

            while (U >= 0 && Y >= 0)
            {
                R = Clip((1.164 * (Y - 16) + 1.596 * (U - 128)));
                G = Clip((1.164 * (Y - 16) - 0.813 * (U - 128) - 0.392 * (V - 128)));
                B = Clip((1.164 * (Y - 16) + 2.017 * (V - 128)));

                // BT.601
                //R = Clip(Y + 1.371 * (U - 128));
                //G = Clip(Y - 0.698 * (U - 128) - 0.336 * (V - 128));
                //B = Clip(Y + 1.732 * (V - 128));

                b.SetPixel(col, row, Color.FromArgb(R, G, B));
                col++;

                R = Clip((1.164 * (Y2 - 16) + 1.596 * (U - 128)));
                G = Clip((1.164 * (Y2 - 16) - 0.813 * (U - 128) - 0.392 * (V - 128)));
                B = Clip((1.164 * (Y2 - 16) + 2.017 * (V - 128)));

                // BT.601
                //R = Clip(Y2 + 1.371 * (U - 128));
                //G = Clip(Y2 - 0.698 * (U - 128) - 0.336 * (V - 128));
                //B = Clip(Y2 + 1.732 * (V - 128));

                b.SetPixel(col, row, Color.FromArgb(R, G, B));
                col++;

                if (col >= 320)
                {
                    row++;
                    col = 0;
                    if (row >= 240)
                        break;
                }

                U = m.ReadByte();
                Y = m.ReadByte();
                V = m.ReadByte();
                Y2 = m.ReadByte();
            }

            PictureBox.Image = b;
            Console.WriteLine("row={0}, col={1}", row, col);
        }
        #endregion
        #region Clip
        private byte Clip(double Value)
        {
            if (Value < 0)
                return 0;
            if (Value > 255)
                return 255;
            return (byte)Value;
        }
        #endregion

        #region OpenPort
        private void OpenPort()
        {
            if (_port == null)
            {
                try
                {
                    _port = new SerialPort(
                        cmbPort.SelectedValue.ToString(),
                        115200, Parity.None, 8, StopBits.One);
                    _port.Open();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(
                        "Fehler beim Öffnen des seriellen ports. Grund:\r\n" + ex.Message, 
                        "Fehler",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }
        #endregion
        #region ClosePort
        private void ClosePort()
        {
            if (_port != null)
            {
                _port.Close();
                _port = null;
            }
        }
        #endregion
        #endregion
    }
}