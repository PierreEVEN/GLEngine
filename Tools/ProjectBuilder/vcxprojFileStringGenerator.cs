using System;
using System.Collections.Generic;
using System.IO;
using System.Text;


namespace ProjectBuilder
{
    struct LibConfig
    {
        public String Configuration;
        public String Plateform;
        public List<String> IncludeFolder;
        public List<String> LibsFolder;
        public List<String> LibNames;
    }

    class vcxprojFileStringGenerator
    {
        private String projectName;
        private String sourceFolder;
        public String binariesFolder = "$(SolutionDir)\\Binaries\\$(Platform)\\$(Configuration)\\$(ProjectName)\\";
        public String outputFolder = "$(SolutionDir)Libs\\";

        public String PlatformToolset = "v142";
        public String WindowTargetPlateformVersion = "10.0.18362.0";
        public String ToolVersion = "15.0";
        public bool bIsStaticLibrary = true;
        public List<LibConfig> libConfig = null;


        public vcxprojFileStringGenerator(String inProjectName, String inSourceFodler)
        {
            projectName = inProjectName;
            sourceFolder = inSourceFodler;
        }

        public void Build()
        {
            String fileString = "";

            AddHeader(ref fileString);
            AddConfigurations(ref fileString);
            SetGlobalParameters(ref fileString);
            fileString += "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n";
            SetConfigurations(ref fileString);
            fileString += "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n";
            AddLabels(ref fileString);
            SetPropertyPath(ref fileString);
            SetItemDefinitions(ref fileString);
            AddIncludes(ref fileString);

            fileString += "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n";
            fileString += "  <ImportGroup Label = \"ExtensionTargets\" >\n";
            fileString += "  </ImportGroup>\n";


            fileString += "</Project>";

            String newFilePath = sourceFolder + "/" + projectName + ".vcxproj";

            if (File.Exists(newFilePath)) File.Delete(newFilePath);

            FileStream stream = new FileStream(newFilePath, FileMode.CreateNew, FileAccess.Write);
            foreach (byte elem in fileString)
            {
                stream.WriteByte(elem);
            }
            stream.Close();
        }

        private String GetLibraryPathString(String inConfig, String inPlateform)
        {
            String result = "";
            if (libConfig != null)
            {
                foreach (LibConfig config in libConfig)
                {
                    if (config.LibsFolder != null && config.Plateform == inPlateform && config.Configuration == inConfig)
                    {
                        foreach (String elem in config.LibsFolder)
                        {
                            result += elem + ";";
                        }
                    }
                }
            }
            return result;
        }
        private String GetLibraryNamesString(String inConfig, String inPlateform)
        {
            String result = "";
            if (libConfig != null)
            {
                foreach (LibConfig config in libConfig)
                {
                    if (config.LibNames != null && config.Plateform == inPlateform && config.Configuration == inConfig)
                    {
                        foreach (String elem in config.LibNames)
                        {
                            result += elem + ";";
                        }
                    }
                }
            }
            return result;
        }
        private String GetIncludeString(String inConfig, String inPlateform)
        {
            String result = "";
            if (libConfig != null)
            {
                foreach (LibConfig config in libConfig)
                {
                    if (config.IncludeFolder != null && config.Plateform == inPlateform && config.Configuration == inConfig)
                    {
                        foreach (String elem in config.IncludeFolder)
                        {
                            result += elem + ";";
                        }
                    }
                }
            }
            return result;
        }
       
        private void AddHeader(ref String fileString)
        {
            fileString += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
            fileString += "<Project DefaultTargets=\"Build\" ToolsVersion=\"" + ToolVersion + "\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";
        }
        private void AddConfigurations(ref String fileString)
        {
            String Configuration;
            String Plateform;
            fileString += "  <ItemGroup Label=\"ProjectConfigurations\">\n";
            for (int i = 0; i < 4; ++i)
            {
                Configuration = (i > 1) ? "Debug" : "Release";
                Plateform = (i % 2 == 0) ? "Win32" : "x64";
                fileString += "    <ProjectConfiguration Include=\"" + Configuration + "|" + Plateform + "\">\n";
                fileString += "      <Configuration>" + Configuration + "</Configuration>\n";
                fileString += "      <Platform>" + Plateform + "</Platform>\n";
                fileString += "    </ProjectConfiguration>\n";
            }
            fileString += "  </ItemGroup>\n";
        }
        private void SetGlobalParameters(ref String fileString)
        {
            fileString += "  <PropertyGroup Label=\"Globals\">\n";
            fileString += "    <VCProjectVersion>" + ToolVersion + "</VCProjectVersion>\n";
            fileString += "    <ProjectGuid>{" + projectName + "}</ProjectGuid>\n";
            fileString += "    <RootNamespace>" + projectName + "</RootNamespace>\n";
            fileString += "    <WindowsTargetPlatformVersion>" + WindowTargetPlateformVersion + "</WindowsTargetPlatformVersion>\n";
            fileString += "  </PropertyGroup>\n";
        }
        private void SetConfigurations(ref String fileString)
        {
            String Configuration = "Debug";
            String Plateform = "Win32";
            for (int i = 0; i < 4; ++i)
            {
                Configuration = (i > 1) ? "Debug" : "Release";
                Plateform = (i % 2 == 0) ? "Win32" : "x64";
                fileString += "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + Configuration + "|" + Plateform + "'\" Label=\"Configuration\">\n";
                if (bIsStaticLibrary) fileString += "    <ConfigurationType>StaticLibrary</ConfigurationType>\n";
                fileString += (Configuration == "Debug") ? "    <UseDebugLibraries>true</UseDebugLibraries>\n" : "    <UseDebugLibraries>false</UseDebugLibraries>\n";
                fileString += "    <PlatformToolset>" + PlatformToolset + "</PlatformToolset>\n";
                fileString += "    <WholeProgramOptimization>false" +
                    "</WholeProgramOptimization>\n";
                fileString += "    <CharacterSet>MultiByte</CharacterSet>\n";
                fileString += "  </PropertyGroup>\n";
            }
        }
        private void AddLabels(ref String fileString)
        {
            fileString += "  <ImportGroup Label=\"ExtensionSettings\">\n";
            fileString += "  </ImportGroup>\n";

            fileString += "  <ImportGroup Label=\"Shared\">\n";
            fileString += "  </ImportGroup>\n";

            fileString += "  <ImportGroup Label=\"PropertySheets\">\n";
            fileString += "    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\n";
            fileString += "  </ImportGroup>\n";

            fileString += "  <PropertyGroup Label=\"UserMacros\" />\n";
        }
        private void SetPropertyPath(ref String fileString)
        {
            String Configuration;
            String Plateform;
            for (int i = 0; i < 4; ++i)
            {
                Configuration = (i > 1) ? "Debug" : "Release";
                Plateform = (i % 2 == 0) ? "Win32" : "x64";

                fileString += "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + Configuration + "|" + Plateform + "'\">\n";
                fileString += "    <IncludePath>" + GetIncludeString(Configuration, Plateform) + "$(IncludePath)</IncludePath>\n";
                fileString += "    <LibraryPath>" + GetLibraryPathString(Configuration, Plateform) + "$(LibraryPath)</LibraryPath>\n";
                fileString += "    <OutDir>" + outputFolder + "</OutDir>\n";
                fileString += "    <IntDir>" + binariesFolder + "</IntDir>\n";
                fileString += "  </PropertyGroup>\n";
            }
        }
        private void SetItemDefinitions(ref String fileString)
        {
            String Configuration;
            String Plateform;
            for (int i = 0; i < 4; ++i)
            {
                Configuration = (i > 1) ? "Debug" : "Release";
                Plateform = (i % 2 == 0) ? "Win32" : "x64";
                fileString += "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)' == '" + Configuration + "|" + Plateform + "'\">\n";
                fileString += "    <ClCompile>\n";
                fileString += "      <WarningLevel>Level3</WarningLevel>\n";
                fileString += "      <Optimization>Disabled</Optimization>\n";
                fileString += "      <SDLCheck>true</SDLCheck>\n";
                fileString += "      <ConformanceMode>true</ConformanceMode>\n";
                fileString += "      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>\n";
                fileString += "      <MultiProcessorCompilation>true</MultiProcessorCompilation>\n";
                fileString += "      <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>\n";
                fileString += "    </ClCompile>\n";
                fileString += "    <Link>\n";
                fileString += "      <AdditionalDependencies>" + GetLibraryNamesString(Configuration, Plateform) + "%(AdditionalDependencies)</AdditionalDependencies>\n";
                fileString += "    </Link>\n";
                fileString += "  </ItemDefinitionGroup>\n";
            }
        }
        private void AddIncludes(ref String fileString)
        {
            fileString += "  <ItemGroup>\n";
            foreach (String file in ProjLibrary.GetFilesInDirectory(sourceFolder, ".h"))
            {
                if (file.Substring(file.Length - 2, 2) == ".h")
                {
                    fileString += "    <ClInclude Include=\"" + file.Substring(sourceFolder.Length + 1, file.Length - sourceFolder.Length - 1) + "\" />\n";
                }
            }
            fileString += "  </ItemGroup>\n";
            fileString += "  <ItemGroup>\n";
            foreach (String file in ProjLibrary.GetFilesInDirectory(sourceFolder, ".cpp"))
            {
                fileString += "    <ClCompile Include=\"" + file.Substring(sourceFolder.Length + 1, file.Length - sourceFolder.Length - 1) + "\" />\n";
            }
            foreach (String file in ProjLibrary.GetFilesInDirectory(sourceFolder, ".c"))
            {
                fileString += "    <ClCompile Include=\"" + file.Substring(sourceFolder.Length + 1, file.Length - sourceFolder.Length - 1) + "\" />\n";
            }
            fileString += "  </ItemGroup>\n";
        }
    }
}
