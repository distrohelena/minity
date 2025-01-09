using Codice.Client.BaseCommands.TubeClient;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Palmmedia.ReportGenerator.Core;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;

namespace ps2unity {
    public static class ExpressionUtil {
        public struct MapVarData {
            public string Name;
            public bool IsValue;
            public string Include;

            public MapVarData(string name, bool value, string include) {
                Name = name;
                IsValue = value;
                Include = include;
            }
        }

        private static Dictionary<string, MapVarData> MapVariableType = new Dictionary<string, MapVarData>()
        {
            { "Vector3", new MapVarData("float3", true, "structs") },
            { "Transform", new MapVarData("GameObject", false, "GameObject") },
        };

        private static Dictionary<string, string> MapFunction = new Dictionary<string, string>()
{
            { "Start", "Start" },
            { "OnEnable", "OnEnable" },
            { "Update", "Tick" },
        };

        public static ClassData InputData = new ClassData() {
            Name = "Input",
            Variables = new List<VariableData> {
                // new VariableData("deltaTime", "float", "DeltaTime", ""),
            }
        };

        public static ClassData TimeData = new ClassData() {
            Name = "Time",
            Variables = new List<VariableData> {
                new VariableData("deltaTime", "float", "DeltaTime", ""),
            }
        };

        public static ClassData MonoBehaviourData = new ClassData() {
            Name = "Transform",
            Variables = new List<VariableData> {
                new VariableData("transform", "Transform", "", "parent"),
                new VariableData("position", "Vector3", "", "GetPosition()", "SetPosition("),
                new VariableData("rotation", "Vector4", "", "GetOrientation()", "SetOrientation("),
                new VariableData("forward", "Vector3", "GetForward()", "SetForward("),
                new VariableData("localEulerAngles", "Vector3", "GetLocalRotation()", ""),
            }
        };

        public static ClassData Vector2Data = new ClassData() {
            Name = "Vector3",
            Variables = new List<VariableData> {
                new VariableData("x", "float", "", ""),
                new VariableData("y", "float", "", ""),
            }
        };

        public static ClassData Vector3Data = new ClassData() {
            Name = "Vector3",
            Variables = new List<VariableData> {
                new VariableData("x", "float", "", ""),
                new VariableData("y", "float", "", ""),
                new VariableData("z", "float", "", ""),
            }
        };

        public static ClassData Vector4Data = new ClassData() {
            Name = "Vector4",
            Variables = new List<VariableData> {
                new VariableData("x", "float", "", ""),
                new VariableData("y", "float", "", ""),
                new VariableData("z", "float", "", ""),
                new VariableData("W", "float", "", ""),
            }
        };

        public static void PreProcessExpression(ProgramData program, ClassData cl, SyntaxNode exp) {
            if (exp is FieldDeclarationSyntax) {
                var field = (FieldDeclarationSyntax)exp;
                VariableDeclarationSyntax declaration = field.Declaration;
                VariableDeclaratorSyntax firstVar = declaration.Variables[0];

                ps2unity.VariableData prop = new ps2unity.VariableData();
                prop.Name = firstVar.Identifier.ToString();
                prop.Type = declaration.Type.ToString();
                cl.Variables.Add(prop);

                if (firstVar.Initializer != null) {
                    prop.Value = firstVar.Initializer.Value.ToString();
                }
            } else if (exp is PropertyDeclarationSyntax) {
                var propDec = (PropertyDeclarationSyntax)exp;

                ps2unity.VariableData prop = new ps2unity.VariableData();
                prop.Name = propDec.Identifier.ToString();
                prop.Type = propDec.Type.ToString();
                cl.Variables.Add(prop);

                if (propDec.Initializer != null) {
                    prop.Value = propDec.Initializer.Value.ToString();
                }
            } else if (exp is MethodDeclarationSyntax) {
                var method = (MethodDeclarationSyntax)exp;

                FunctionData func = new FunctionData();
                func.Name = method.Identifier.ToString();
                func.ReturnType = method.ReturnType.ToString();
                cl.Functions.Add(func);
                func.Declaration = method;
            }
        }

        public static bool IsOpenInvocation(string invo) {
            int index = invo.LastIndexOf("(");
            if (index == -1) {
                return false;
            }

            int ending = invo.LastIndexOf(")");

            return ending < index;
        }

        public static void ProcessFnExpression(ProgramData program, ClassData cl, SyntaxNode exp, FunctionData data) {
            if (exp is ExpressionStatementSyntax) {
                var expression = (ExpressionStatementSyntax)exp;
                ProcessFnExpression(program, cl, expression.Expression, data);
            } else if (exp is AssignmentExpressionSyntax) {
                var assignment = (AssignmentExpressionSyntax)exp;

                data.ReadGetFlag = false;
                if (assignment.Left is MemberAccessExpressionSyntax) {
                    MemberAccessExpressionSyntax member = (MemberAccessExpressionSyntax)assignment.Left;

                    data.ReadGetFlag = true;
                    ProcessFnExpression(program, cl, member.Expression, data);
                    data.ReadGetFlag = false;

                    if (member.OperatorToken.ToString() == ".") {
                        data.Operations.Add("->");
                        ProcessFnExpression(program, cl, member.Name, data);
                    }

                    string lastOp = data.GetLastOperation();
                    if (!lastOp.Contains("(")) {
                        data.Operations.Add(" = ");
                    }
                } else if (assignment.Left is ElementAccessExpressionSyntax) {
                    throw new Exception("Error Assign");
                } else if (assignment.Left is IdentifierNameSyntax) {
                    IdentifierNameSyntax ident = (IdentifierNameSyntax)assignment.Left;
                    data.Operations.Add($"{ident.Identifier.ToString()} = ");
                }

                data.ReadGetFlag = true;
                ProcessFnExpression(program, cl, assignment.Right, data);

                string lastLine = data.GetLastLine();
                if (IsOpenInvocation(lastLine)) {
                    data.Operations.Add(");\n");
                } else {
                    data.Operations.Add(";\n");
                }


            } else if (exp is BinaryExpressionSyntax) {
                BinaryExpressionSyntax expression = (BinaryExpressionSyntax)exp;

                ProcessFnExpression(program, cl, expression.Left, data);
                data.Operations.Add($" {expression.OperatorToken.Text} ");
                ProcessFnExpression(program, cl, expression.Right, data);
            } else if (exp is MemberAccessExpressionSyntax) {
                MemberAccessExpressionSyntax member = (MemberAccessExpressionSyntax)exp;
                string[] access = member.ToString().Split('.');

                if (access.Length == 1) {
                    data.Operations.Add(access[0]);
                } else {
                    ClassData context = cl;
                    bool isStatic = false;
                    for (int i = 0; i < access.Length; i++) {
                        string accessWord = access[i];
                        VariableData classVar = context.GetVar(accessWord);

                        if (classVar == null) {
                            // static access
                            context = program.Classes.FirstOrDefault(c => c.Name == accessWord);
                            data.Operations.Add($"{accessWord}::");
                            cl.CodeIncludes.Add(accessWord);
                            isStatic = true;
                        } else {
                            if (isStatic) {
                                data.Operations.Add(classVar.Value);
                            } else {
                                context = program.Classes.FirstOrDefault(c => c.Name == classVar.Type);
                                if (context == null) {
                                    Debugger.Break();
                                    UnityEngine.Debug.Log("No Type  " + classVar.Type);
                                }

                                if (string.IsNullOrEmpty(classVar.Getter)) {
                                    data.Operations.Add(access[i]);
                                } else {
                                    data.Operations.Add(classVar.Getter);
                                }
                            }

                            if (i != access.Length - 1) {
                                if (MapVariableType.TryGetValue(classVar.Type, out MapVarData mdata)) {
                                    if (!mdata.IsValue) {
                                        data.Operations.Add("->");
                                    }
                                } else {
                                    data.Operations.Add("->");
                                }
                            }
                        }
                    }
                }

            } else if (exp is LocalDeclarationStatementSyntax) {
                LocalDeclarationStatementSyntax local = (LocalDeclarationStatementSyntax)exp;

                if (local.Declaration.Variables.Count > 1) {
                    throw new NotImplementedException();
                }

                string type = local.Declaration.Type.ToString();
                if (MapVariableType.TryGetValue(type, out MapVarData value)) {
                    type = value.Name;
                }

                VariableDeclaratorSyntax firstVar = local.Declaration.Variables[0];

                if (firstVar.Initializer == null) {
                    data.Operations.Add($"{type} {firstVar.Identifier};");
                } else {
                    data.Operations.Add($"{type} {firstVar.Identifier} = ");

                    var init = firstVar.Initializer.Value;
                    ProcessFnExpression(program, cl, init, data);
                }

                string lastOp = data.GetLastOperation();
                if (!lastOp.EndsWith(";\n") &&
                    !lastOp.EndsWith(";")) {
                    data.Operations.Add(";\n");
                }

            } else if (exp is IdentifierNameSyntax) {
                IdentifierNameSyntax identifier = (IdentifierNameSyntax)exp;

                VariableData classVar = cl.GetVar(identifier.ToString());

                if (classVar == null) {
                    // throw new Exception("Class is null");
                    data.Operations.Add(identifier.ToString());
                } else {
                    if (data.ReadGetFlag) {
                        if (string.IsNullOrEmpty(classVar.Getter)) {
                            data.Operations.Add($"{classVar.Name}");
                        } else {
                            data.Operations.Add($"{classVar.Getter}");
                        }
                    } else {
                        if (string.IsNullOrEmpty(classVar.Setter)) {
                            data.Operations.Add($"{classVar.Name}");
                        } else {
                            data.Operations.Add($"{classVar.Setter}");
                        }
                    }
                }
            } else if (exp is InvocationExpressionSyntax) {
                InvocationExpressionSyntax invocation = (InvocationExpressionSyntax)exp;
                var expression = invocation.Expression;

                bool first = true;
                bool isStatic = false;
                string prePend = "";
                foreach (SyntaxNode node in expression.ChildNodes()) {
                    if (node is InvocationExpressionSyntax) {
                        InvocationExpressionSyntax inv = (InvocationExpressionSyntax)node;

                        //if (string.IsNullOrEmpty(prePend)) {
                        //    ProcessInvocation(inv.Expression.ToString(), context, operations, inv.ArgumentList.Arguments);
                        //} else {
                        //    ProcessInvocation(prePend + "." + inv.Expression.ToString(), context, operations, inv.ArgumentList.Arguments);
                        //    prePend = string.Empty;
                        //}
                    } else if (node is IdentifierNameSyntax) {
                        IdentifierNameSyntax identifier = (IdentifierNameSyntax)node;

                        if (first) {
                            prePend = identifier.ToString();

                            VariableData varData = cl.GetVar(prePend);
                            if (varData == null) {
                                isStatic = true;
                                if (MapVariableType.TryGetValue(prePend, out MapVarData value)) {
                                    prePend = value.Name;
                                }
                            } else {
                                if (data.ReadGetFlag) {
                                    prePend = varData.Getter;
                                } else {
                                    prePend = varData.Setter;
                                }

                                if (string.IsNullOrEmpty(prePend)) {
                                    prePend = varData.Name;
                                }
                            }

                        } else {

                            if (string.IsNullOrEmpty(prePend)) {
                                data.Operations.Add(identifier.ToString() + "(");
                            } else {
                                if (isStatic) {
                                    data.Operations.Add(prePend + "::" + identifier.ToString() + "(");
                                } else {
                                    data.Operations.Add(prePend + "->" + identifier.ToString() + "(");
                                }
                                prePend = string.Empty;
                            }

                            int count = invocation.ArgumentList.Arguments.Count;
                            int i = 0;
                            foreach (var arg in invocation.ArgumentList.Arguments) {
                                ProcessFnExpression(program, cl, arg, data);
                                if (i != count - 1) {
                                    data.Operations.Add(", ");
                                }
                                i++;
                            }
                            data.Operations.Add(");\n");
                        }
                    } else if (node is MemberAccessExpressionSyntax) {
                        MemberAccessExpressionSyntax memberAccess = (MemberAccessExpressionSyntax)node;
                        //ProcessMemberAcccessExpression(memberAccess, context, operations);
                        //context.SetInvocationSub();
                    } else if (node is ElementAccessExpressionSyntax) {
                        // array access
                        //ReadArrayVarOp readArr = new ReadArrayVarOp();

                        ElementAccessExpressionSyntax access = (ElementAccessExpressionSyntax)node;
                        //AssingArrayAccess(readArr, access.Expression.ToString(), access.ArgumentList, context);

                        //operations.Add(readArr);
                    } else if (node is PredefinedTypeSyntax) {
                        // static member of predefined type
                        PredefinedTypeSyntax predefinedType = (PredefinedTypeSyntax)node;
                        //if (first) {
                        //    // static invocation identifier
                        //    prePend = predefinedType.Keyword.ToString();
                        //} else {
                        //    if (string.IsNullOrEmpty(prePend)) {
                        //        ProcessInvocation(predefinedType.Keyword.ToString(), context, operations, invocation.ArgumentList.Arguments);
                        //    } else {
                        //        ProcessInvocation(prePend + "." + predefinedType.Keyword.ToString(), context, operations, invocation.ArgumentList.Arguments);
                        //        prePend = string.Empty;
                        //    }
                        //}
                    } else if (node is ParenthesizedExpressionSyntax) {
                        // ( ) parenthesis
                        ParenthesizedExpressionSyntax parenthesized = (ParenthesizedExpressionSyntax)node;
                        //ProcessExpression(context, operations, parenthesized.Expression);
                    } else {
                        Debugger.Break();
                    }
                    first = false;
                }
            } else if (exp is ArgumentSyntax) {
                ArgumentSyntax arg = (ArgumentSyntax)exp;
                ProcessFnExpression(program, cl, arg.Expression, data);
            } else if (exp is LiteralExpressionSyntax) {
                LiteralExpressionSyntax literal = (LiteralExpressionSyntax)exp;
                data.Operations.Add(literal.ToString());

            } else {
                UnityEngine.Debug.Log(exp.GetType().Name);
                Debugger.Break();
            }
        }

        public static void ProcessDeclaration(ClassData cl, ProgramData program, StreamWriter header, StreamWriter source) {
            header.WriteLine("#pragma once");

            List<string> includes = new List<string>();
            for (int i = 0; i < cl.Variables.Count; i++) {
                VariableData prop = cl.Variables[i];
                if (MapVariableType.TryGetValue(prop.Type, out MapVarData data)) {
                    includes.Add(data.Include);
                } else {
                    // check for user classes
                    ClassData propCl = program.Classes.FirstOrDefault(c => c.Name == prop.Type);
                    if (propCl != null) {
                        includes.Add(propCl.Name);
                    }
                }
            }
            includes = includes.Distinct().ToList();

            List<string> codeIncludes = cl.CodeIncludes;
            codeIncludes = codeIncludes.Distinct().ToList();

            for (int i = 0; i < includes.Count; i++) {
                header.WriteLine("#include \"" + includes[i] + ".h\"");
            }

            if (string.IsNullOrEmpty(cl.BaseClass)) {
                header.WriteLine("class " + cl.Name);
            } else {
                header.WriteLine("#include \"" + cl.BaseClass + ".h\"");
                header.WriteLine("class " + cl.Name + " : public " + cl.BaseClass);
            }
            header.WriteLine("{");

            source.WriteLine("#include \"" + cl.Name + ".h\"");
            for (int i = 0; i < codeIncludes.Count; i++) {
                source.WriteLine("#include \"" + codeIncludes[i] + ".h\"");
            }

            source.WriteLine("");

            if (cl.Functions.Count > 0 || cl.Variables.Count > 0) {
                header.WriteLine("public:");
            }

            for (int i = 0; i < cl.Variables.Count; i++) {
                ps2unity.VariableData prop = cl.Variables[i];

                string type = prop.Type;
                if (MapVariableType.TryGetValue(type, out MapVarData data)) {
                    type = data.Name;

                    if (!data.IsValue) {
                        type += "*";
                    }
                } else {
                    // check for user classes
                    ClassData propCl = program.Classes.FirstOrDefault(c => c.Name == type);
                    if (propCl != null) {
                        type += "*";
                    }
                }

                string value = "";
                if (!string.IsNullOrEmpty(prop.Value)) {
                    value = $" = {prop.Value}";
                }

                header.WriteLine($"    {type} {prop.Name}{value};");
            }
            header.WriteLine($"");

            for (int i = 0; i < cl.Functions.Count; i++) {
                FunctionData func = cl.Functions[i];

                string paramData = "";

                if (func.InVars.Count == 0) {
                    paramData = "void";
                }

                string name = func.Name;
                string overrideStr = "";
                if (MapFunction.TryGetValue(func.Name, out string replacement)) {
                    name = replacement;
                    overrideStr = " override";
                }

                header.WriteLine($"    {func.ReturnType} {name}({paramData}){overrideStr};");

                source.WriteLine($"{func.ReturnType} {cl.Name}::{name}({paramData}) {{");

                for (int j = 0; j < func.Operations.Count; j++) {
                    source.Write(func.Operations[j]);
                }

                source.WriteLine("\n");
                source.WriteLine("}");
                source.WriteLine("");
            }

            header.WriteLine("};");
        }

        public static void PreProcessExpression(ProgramData program, SyntaxNode exp) {
            if (exp is ClassDeclarationSyntax) {
                // class Name
                ClassDeclarationSyntax classDecl = (ClassDeclarationSyntax)exp;
                string name = classDecl.Identifier.ToString();

                ClassData cl = new ClassData();
                cl.Name = name;
                cl.Declaration = classDecl;
                program.Classes.Add(cl);

                if (classDecl.BaseList != null) {
                    foreach (var baseType in classDecl.BaseList.ChildNodes()) {
                        if (baseType is SimpleBaseTypeSyntax) {
                            SimpleBaseTypeSyntax baseSyntax = (SimpleBaseTypeSyntax)baseType;
                            string type = baseSyntax.Type.ToString();

                            if (type == "MonoBehaviour") {
                                cl.IsComponent = true;
                                cl.BaseClass = "GameComponent";
                                cl.BaseData = MonoBehaviourData;
                            } else {
                                Debugger.Break();
                            }
                        }
                    }
                }

                foreach (MemberDeclarationSyntax memberSyntax in cl.Declaration.Members) {
                    PreProcessExpression(program, cl, memberSyntax);
                }
            }
        }

        public static void ProcessProgram(ProgramData program) {
            for (int i = 0; i < program.Classes.Count; i++) {
                ClassData cl = program.Classes[i];
                if (cl.Declaration == null) {
                    continue;
                }

                for (int j = 0; j < cl.Functions.Count; j++) {
                    FunctionData fnData = cl.Functions[j];
                    foreach (SyntaxNode node in fnData.Declaration.Body.ChildNodes()) {
                        ProcessFnExpression(program, cl, node, fnData);
                    }
                }

            }

        }


    }
}
