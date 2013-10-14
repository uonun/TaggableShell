using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace Tool.CloseAllExplorer
{
    class Program
    {
        static void Main(string[] args)
        {
            var list = Process.GetProcessesByName("explorer");
            foreach (var item in list)
            {
                item.Kill();
            }
        }
    }
}
