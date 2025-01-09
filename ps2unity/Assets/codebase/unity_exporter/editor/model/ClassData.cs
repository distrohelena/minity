using Microsoft.CodeAnalysis.CSharp.Syntax;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ps2unity
{
    public class ClassData
    {
        public ClassData BaseData { get; set; }
        public virtual string Name { get; set; }
        public bool IsComponent { get; set; }
        public string BaseClass { get; set; }

        public List<FunctionData> Functions { get; set; }
        public List<VariableData> Variables { get; set; }

        public ClassDeclarationSyntax Declaration { get; set; }
        public List<string> CodeIncludes { get; set; }

        public ClassData() {
            Functions = new List<FunctionData>();
            Variables = new List<VariableData>();
            CodeIncludes = new List<string>();
        }

        public VariableData GetVar(string name) {
            // first search for variable in class
            VariableData classVar = Variables.FirstOrDefault(c => c.Name == name);

            var baseClass = BaseData;
            // search for all base classes
            if (classVar == null) {
                for (; ; ) {
                    if (baseClass == null) {
                        break;
                    } else {
                        classVar = baseClass.Variables.FirstOrDefault(c => c.Name == name);
                        if (classVar == null) {
                            baseClass = baseClass.BaseData;
                        } else {
                            break;
                        }
                    }
                }
            }

            return classVar;
        }
    }
}
