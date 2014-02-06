using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace Tool.CloseAllExplorer
{
    class Program
    {
        // although the CMD taskkill /f /im explorer.exe chould do the same work
        // but the explorer.exe could not restored after killed
        // so, i write this to kill explorer.
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
