using System;
using System.Collections.Generic;
using System.IO;
using System.Text;


namespace ProjectBuilder
{

    class vcxprojFileStringGenerator
    {

        public static String BuildVcxprojString(ProjectStruct inProject)
        {
            ProjLibrary.BeginXmlEdition();
            ProjLibrary.BeginXmlCategory("Project", "DefaultTargets=\"Build\" ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"");
            {
                AddConfigurations();
                SetProjectConfigs(inProject);
                ProjLibrary.AddXmlValue("Import", "", "Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\"");
                SetConfigurationParameters(inProject);
                ProjLibrary.AddXmlValue("Import", "", "Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\"");
                ImportLabels();
                SetPropertyPath(inProject);
                SetCompilationValues(inProject);
                IncludeSourceFiles(inProject);
                ProjLibrary.AddXmlValue("Import", "", "Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\"");
                ProjLibrary.AddXmlValue("ImportGroup", "", "Label=\"ExtensionTargets\"");
            }
            ProjLibrary.EndXmlCategory("Project");

            return ProjLibrary.GetXmlString();
        }

        private static void AddConfigurations()
        {
            ProjLibrary.BeginXmlCategory("ItemGroup", "Label=\"ProjectConfigurations\"");
            {
                for (int i = 0; i < 4; ++i)
                {
                    String Configuration = (i > 1) ? "Debug" : "Release";
                    String Plateform = (i % 2 == 0) ? "Win32" : "x64";

                    ProjLibrary.BeginXmlCategory("ProjectConfiguration", "Include=\"" + Configuration + "|" + Plateform + "\"");
                    {
                        ProjLibrary.AddXmlValue("Configuration", Configuration);
                        ProjLibrary.AddXmlValue("Platform", Plateform);
                    }
                    ProjLibrary.EndXmlCategory("ProjectConfiguration");
                }
            }
            ProjLibrary.EndXmlCategory("ItemGroup");
        }
        private static void SetProjectConfigs(ProjectStruct inProject)
        {
            SolutionStruct solutionData = SolutionAnalyzer.Get().solutionData;
            ProjLibrary.BeginXmlCategory("PropertyGroup", "Label =\"Globals\"");
            {
                ProjLibrary.AddXmlValue("VCProjectVersion", solutionData.ToolVersion);
                ProjLibrary.AddXmlValue("ProjectGuid", inProject.ProjectName);
                ProjLibrary.AddXmlValue("RootNamespace", inProject.ProjectName);
                ProjLibrary.AddXmlValue("WindowsTargetPlatformVersion", solutionData.WindowTargetPlateformVersion);
            }
            ProjLibrary.EndXmlCategory("PropertyGroup");
        }
        private static void SetConfigurationParameters(ProjectStruct inProject)
        {
            SolutionStruct solutionData = SolutionAnalyzer.Get().solutionData;
            for (int i = 0; i < 4; ++i)
            {
                String Configuration = (i > 1) ? "Debug" : "Release";
                String Plateform = (i % 2 == 0) ? "Win32" : "x64";

                ProjLibrary.BeginXmlCategory("PropertyGroup", "Condition=\"'$(Configuration)|$(Platform)'=='" + Configuration + "|" + Plateform + "'\" Label=\"Configuration\"");
                {
                    if (inProject.ProjectType == "StaticLibrary") ProjLibrary.AddXmlValue("ConfigurationType", "StaticLibrary");
                    if (Configuration == "Debug")  ProjLibrary.AddXmlValue("UseDebugLibraries", "true");
                    if (Configuration != "Debug") ProjLibrary.AddXmlValue("UseDebugLibraries", "false");
                    ProjLibrary.AddXmlValue("PlatformToolset", solutionData.PlatformToolset);
                    ProjLibrary.AddXmlValue("WholeProgramOptimization", "false");
                    ProjLibrary.AddXmlValue("CharacterSet", "MultiByte");
                    ProjLibrary.AddXmlValue("CharacterSet", "MultiByte");
                }
                ProjLibrary.EndXmlCategory("PropertyGroup");
            }
        }
        private static void ImportLabels()
        {
            ProjLibrary.AddXmlValue("ImportGroup", "", "Label=\"ExtensionSettings\"");
            ProjLibrary.AddXmlValue("ImportGroup", "", "Label=\"Shared\"");
            ProjLibrary.BeginXmlCategory("ImportGroup", "Label=\"PropertySheets\"");
            {
                ProjLibrary.AddXmlValue("Import", "", "Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"");
            }
            ProjLibrary.EndXmlCategory("ImportGroup");
            ProjLibrary.AddXmlValue("PropertyGroup", "", "Label=\"UserMacros\"");
        }
        private static void SetPropertyPath(ProjectStruct inProject)
        {
            SolutionStruct solutionData = SolutionAnalyzer.Get().solutionData;
            for (int i = 0; i < 4; ++i)
            {
                String Configuration = (i > 1) ? "Debug" : "Release";
                String Plateform = (i % 2 == 0) ? "Win32" : "x64";

                ProjLibrary.BeginXmlCategory("PropertyGroup", "Condition=\"'$(Configuration)|$(Platform)'=='" + Configuration + "|" + Plateform + "'\"");
                {
                    ProjLibrary.AddXmlValue("IncludePath", SolutionAnalyzer.StackStringList(SolutionAnalyzer.Get().GetIncludes(inProject.ProjectName)) + "$(IncludePath)");
                    ProjLibrary.AddXmlValue("LibraryPath", "$(SolutionDir)" + solutionData.LibraryOutputFolder + "\\" + Configuration + "\\" + Plateform + "\\" + inProject.ProjectName + ";$(LibraryPath)");
                    ProjLibrary.AddXmlValue("OutDir", "$(SolutionDir)" + (inProject.ProjectType == "StaticLibrary" ? solutionData.LibraryOutputFolder : solutionData.ExecutableOutputFolder) + "\\" + Configuration + "\\" + Plateform + "\\" + inProject.ProjectName + "\\");
                    ProjLibrary.AddXmlValue("IntDir", "$(SolutionDir)" + solutionData.BinariesFolder + "\\" + Configuration + "\\" + Plateform + "\\" + inProject.ProjectName + "\\");
                }
                ProjLibrary.EndXmlCategory("PropertyGroup");
            }
        }
        private static void SetCompilationValues(ProjectStruct inProject)
        {
            SolutionStruct solutionData = SolutionAnalyzer.Get().solutionData;
            for (int i = 0; i < 4; ++i)
            {
                String Configuration = (i > 1) ? "Debug" : "Release";
                String Plateform = (i % 2 == 0) ? "Win32" : "x64";
                ProjLibrary.BeginXmlCategory("ItemDefinitionGroup", "Condition=\"'$(Configuration)|$(Platform)' == '" + Configuration + "|" + Plateform + "'\"");
                {
                    ProjLibrary.BeginXmlCategory("ClCompile");
                    {
                        ProjLibrary.AddXmlValue("WarningLevel", "Level3");
                        ProjLibrary.AddXmlValue("Optimization", "Disabled");
                        ProjLibrary.AddXmlValue("SDLCheck", "true");
                        ProjLibrary.AddXmlValue("ConformanceMode", "true");
                        ProjLibrary.AddXmlValue("RuntimeLibrary", "MultiThreadedDebugDLL");
                        ProjLibrary.AddXmlValue("MultiProcessorCompilation", "true");
                        ProjLibrary.AddXmlValue("FavorSizeOrSpeed", "Speed");
                    }
                    ProjLibrary.EndXmlCategory("ClCompile");
                    ProjLibrary.BeginXmlCategory("Link");
                    {
                        try
                        {
                            ProjLibrary.AddXmlValue("AdditionalDependencies", SolutionAnalyzer.StackStringList(SolutionAnalyzer.Get().GetLibraries(inProject.ProjectName, Configuration, Plateform)) + "%(AdditionalDependencies)");
                        }
                        catch (Exception e)
                        {
                            Console.WriteLine(e.Message + "\nBuild libraries then rebuild solution files before compiling executables");
                        }
                    }
                    ProjLibrary.EndXmlCategory("Link");
                }
                ProjLibrary.EndXmlCategory("ItemDefinitionGroup");
            }
        }
        private static void IncludeSourceFiles(ProjectStruct inProject)
        {
            ProjLibrary.BeginXmlCategory("ItemGroup");
            {
                foreach (String file in ProjLibrary.GetFilesInDirectory(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), ".h"))
                {
                    if (file.Substring(file.Length - 2, 2) == ".h")
                    {
                        ProjLibrary.AddXmlValue("ClInclude", "", "Include=\"" + Path.GetRelativePath(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), file) + "\"");
                    }
                }
            }
            ProjLibrary.EndXmlCategory("ItemGroup");

            ProjLibrary.BeginXmlCategory("ItemGroup");
            {
                foreach (String file in ProjLibrary.GetFilesInDirectory(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), ".cpp"))
                {
                    ProjLibrary.AddXmlValue("ClCompile", "", "Include=\"" + Path.GetRelativePath(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), file) + "\"");
                }
                foreach (String file in ProjLibrary.GetFilesInDirectory(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), ".c"))
                {
                    ProjLibrary.AddXmlValue("ClCompile", "", "Include=\"" + Path.GetRelativePath(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), file) + "\"");
                }
            }
            ProjLibrary.EndXmlCategory("ItemGroup");
        }
    }
}
