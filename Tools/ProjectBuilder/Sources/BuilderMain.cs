using System;
using System.IO;

namespace ProjectBuilder
{
    class BuilderMain
    {
        static void Main(string[] args)
        {
            SolutionAnalyzer.SetSolutionAbsolutePath(args[0]);// "C:\\Users\\Pierre\\Documents\\GLEngine");
            foreach(ProjectStruct proj in SolutionAnalyzer.Get().Projects)
            {
                try
                {
                    ProjectBuilder.WriteProjectFile(vcxprojFileStringGenerator.BuildVcxprojString(proj), Path.GetDirectoryName(proj.ProjectFileAbsolutePath) + "\\" + proj.ProjectName + ".vcxproj");
                    ProjectBuilder.WriteProjectFile(userFileStringGenerator.BuildUserFileString(proj), Path.GetDirectoryName(proj.ProjectFileAbsolutePath) + "\\" + proj.ProjectName + ".vcxproj.user");
                    ProjectBuilder.WriteProjectFile(filtersFileGenerator.BuildFilterString(proj), Path.GetDirectoryName(proj.ProjectFileAbsolutePath) + "\\" + proj.ProjectName + ".vcxproj.filters");
                    Console.WriteLine("=> Successfully created " + proj.ProjectName + " file");

                }
                catch(Exception e)
                {
                    Console.WriteLine("Failed to build proj " + proj.ProjectName + " :\n" + e.Message);
                }
            }
         }
    }
}
