using System;
using System.Collections.Generic;
using System.Text;

namespace ProjectBuilder
{
    class userFileStringGenerator
    {
        public static String BuildUserFileString(ProjectStruct inProject)
        {
            SolutionStruct solutionData = SolutionAnalyzer.Get().solutionData;
            ProjLibrary.BeginXmlEdition();
            ProjLibrary.BeginXmlCategory("Project", "ToolsVersion=\"Current\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"");
            {
                for (int i = 0; i < 4; ++i)
                {
                    String Configuration = (i > 1) ? "Debug" : "Release";
                    String Plateform = (i % 2 == 0) ? "Win32" : "x64";

                    ProjLibrary.BeginXmlCategory("PropertyGroup", "Condition=\"'$(Configuration)|$(Platform)'=='" + Configuration + "|" + Plateform + "'\" Label=\"Configuration\"");
                    {
                        ProjLibrary.AddXmlValue("LocalDebuggerWorkingDirectory", "$(SolutionDir)" + solutionData.ExecutableOutputFolder + "\\" + Configuration + "\\" + Plateform + "\\" + inProject.ProjectName + "\\");
                        ProjLibrary.AddXmlValue("DebuggerFlavor", "WindowsLocalDebugger");
                    }
                    ProjLibrary.EndXmlCategory("PropertyGroup");
                }
            }
            ProjLibrary.EndXmlCategory("Project");
            return ProjLibrary.GetXmlString();
        }
    }
}
