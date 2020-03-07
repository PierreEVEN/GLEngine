using System;
using System.IO;

namespace ProjectBuilder
{
    class Program
    {
        static void Main(string[] args)
        {
            ProjectBuilder proj = new ProjectBuilder("../../../../../Modules/");
            proj.Build();


         }
    }
}
