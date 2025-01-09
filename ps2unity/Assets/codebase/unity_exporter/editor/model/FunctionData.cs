using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CodeAnalysis.CSharp.Syntax;

namespace ps2unity
{
    public class FunctionData
    {

        public string Name { get; set; }
        public string ReturnType { get; set; }
        public List<string> Operations { get; set; } = new List<string>();

        public List<VariableData> InVars { get; set; } = new List<VariableData>();

        public MethodDeclarationSyntax Declaration { get; set; }

        public bool ReadGetFlag {get; set;}

        public string GetLastOperation() {
            if (Operations.Count == 0) {
                return string.Empty;
            }
            return Operations[Operations.Count - 1];
        }

        public string GetLastLine() {
            List<string> lastLine = new List<string>();

            for (int i = Operations.Count - 1; i >= 0; i--) {
                string str = Operations[i];

                if (str.Contains('\n')) {
                    string[] division = str.Split('\n');
                    lastLine.Add(division[division.Length - 1]);
                    break;
                } else {
                    lastLine.Add(str);
                }
            }

            string finalLine = "";
            for (int i = lastLine.Count - 1; i >= 0; i--) {
                finalLine += lastLine[i];
            }

            return finalLine;
        }
    }
}
