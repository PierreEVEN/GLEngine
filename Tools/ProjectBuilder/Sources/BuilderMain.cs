using System;
using System.IO;

namespace ProjectBuilder
{
    class BuilderMain
    {
        static void Main(string[] args)
        {
            if (args.Length == 1)
            {
                Console.WriteLine("starting building on " + args[0]);
                ProjectBuilder proj = new ProjectBuilder(args[0]);//"../../../../../Modules/");
                proj.Build();
            }
            else
            {
                ProjectBuilder proj = new ProjectBuilder("../../../../../Modules/");
                proj.Build();
            }
         }
    }
}
