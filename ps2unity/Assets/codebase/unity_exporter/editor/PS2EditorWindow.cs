using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.CodeAnalysis.Formatting;
using Microsoft.CodeAnalysis.Text;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;
using Scene = UnityEngine.SceneManagement.Scene;

namespace ps2unity {
    public class PS2EditorWindow : EditorWindow {
        private const string ExporterExportPath = "Minity_ExporterPath";
        private const string ExporterCodePath = "Minity_CoderPath";

        private Vector2 scrollPos;
        private MinityPlatform platform = MinityPlatform.PlayStation2;

        // path
        private string exportPath;
        private string codePath;

        // tesselation
        private bool tesselation;
        private float tesselationMinArea = 50;

        // lightmap
        private LightmapMode lightmaps;
        private float lightmapRelFStops;
        private bool lightmapExposureVisible;
        private bool lightmapExposureBuilt;
        private int lightmapResolution = 512;

        private bool exportCode;

        private ExportStats stats;
        private Dictionary<int, List<MeshRenderer>> lightmapMap;
        private Dictionary<object, AssetData> data;

        [NonSerialized]
        private Rect border = new Rect(5, 5, 10, 10);

        public PS2EditorWindow() {
            exportPath = EditorPrefs.GetString(ExporterExportPath, "");
            codePath = EditorPrefs.GetString(ExporterCodePath, "");
        }

        [MenuItem("Window/Minity Exporter")]
        public static void ShowExample() {
            PS2EditorWindow wnd = GetWindow<PS2EditorWindow>();
            wnd.titleContent = new GUIContent("Minity Exporter");
        }

        private void OnGUI() {
            Rect rect = this.position;
            Rect showArea = new Rect(border.x, border.y, rect.width - border.width, rect.height - border.height);

            GUILayout.BeginArea(showArea);
            scrollPos = GUILayout.BeginScrollView(scrollPos);

            GUILayout.BeginHorizontal();
            GUILayout.Label("Minity Exporter v0.0.1 ", EditorStyles.boldLabel);
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            exportPath = EditorGUILayout.TextField("Export Folder", exportPath);
            if (GUILayout.Button("...")) {
                exportPath = EditorUtility.OpenFolderPanel("Select Export Folder", "", "");
                EditorPrefs.SetString(ExporterExportPath, exportPath);
            }
            GUILayout.EndHorizontal();

            exportCode = EditorGUILayout.Toggle("Export Code", exportCode);

            if (exportCode) {
                GUILayout.BeginHorizontal();
                codePath = EditorGUILayout.TextField("Minity Folder", exportPath);
                if (GUILayout.Button("...")) {
                    codePath = EditorUtility.OpenFolderPanel("Select Minity Folder", "", "");
                    EditorPrefs.SetString(ExporterCodePath, codePath);
                }
                GUILayout.EndHorizontal();
            }

            platform = (MinityPlatform)EditorGUILayout.EnumPopup("Platform", platform);
            tesselation = EditorGUILayout.Toggle("Tesselation", tesselation);
            if (tesselation) {
                tesselationMinArea = EditorGUILayout.FloatField("Min Tesselation Area", tesselationMinArea);
            }

            lightmaps = (LightmapMode)EditorGUILayout.EnumPopup("Lightmaps", lightmaps);

            if (lightmaps == LightmapMode.BruteForce) {
                lightmapResolution = EditorGUILayout.IntField("Lightmap Resolution", lightmapResolution);

                float newFStop = EditorGUILayout.Slider("Lightmap Relative F-Stops", lightmapRelFStops, -5, 5);
                bool update = false;
                if (Math.Abs(newFStop - lightmapRelFStops) > 0.0001f) {
                    lightmapRelFStops = newFStop;
                    update = true;
                }

                lightmapExposureVisible = EditorGUILayout.Foldout(lightmapExposureVisible, "Preview Exposure");
                if (lightmapExposureVisible) {
                    if (update || !lightmapExposureBuilt) {
                        lightmapExposureBuilt = LightmapExporter.ProcessExposure(0, lightmapRelFStops);
                    }

                    if (lightmapExposureBuilt) {
                        Texture2D preview = ExporterResources.TempRenderTexture;
                        int texSize = (int)(showArea.width * 0.4);
                        Rect tex = GUILayoutUtility.GetRect(texSize, texSize + 30);

                        GUI.Label(new Rect(tex.x + 20, tex.y + 5, texSize, texSize), "Lightmap Preview");
                        GUI.DrawTexture(new Rect(tex.x + 20, tex.y + 30, texSize, texSize), preview);
                    } else {
                        GUILayout.Label("No Lightmap Preview");
                    }
                }
            }


            if (GUILayout.Button("Export")) {
                Export();
            }

            GUILayout.EndScrollView();
            GUILayout.EndArea();
        }

        private void PreProcessObjs(GameObject[] gos) {
            lightmapMap = new Dictionary<int, List<MeshRenderer>>();

            for (int i = 0; i < gos.Length; i++) {
                GameObject go = gos[i];
                PreProcessObjsChild(go.transform);
            }
        }

        private void PreProcessObjsChild(Transform go) {
            Component[] comps = go.GetComponents(typeof(Component));

            int totalComps = comps.Length;
            for (int i = 0; i < comps.Length; i++) {
                Component comp = comps[i];

                if (comp is MeshRenderer) {
                    MeshRenderer meshRen = (MeshRenderer)comp;

                    List<MeshRenderer> list;
                    if (!lightmapMap.TryGetValue(meshRen.lightmapIndex, out list)) {
                        list = new List<MeshRenderer>();
                        lightmapMap.Add(meshRen.lightmapIndex, list);
                    }
                    list.Add(meshRen);
                }
            }

            foreach (Transform tr in go) {
                PreProcessObjsChild(tr);
            }
        }

        private void WriteObjects(GameObject[] gos, BinaryWriter writer, BinaryWriter assetsWriter, Dictionary<object, string> assets, ProgramData program) {
            writer.Write((uint)gos.Length);

            for (int i = 0; i < gos.Length; i++) {
                GameObject go = gos[i];
                WriteObjectsChild(go.transform, writer, assetsWriter, assets, program);
            }
        }

        static float CalculateTriangleArea(Vector3 A, Vector3 B, Vector3 C) {
            Vector3 AB = B - A;
            Vector3 AC = C - A;

            Vector3 crossProduct = Vector3.Cross(AB, AC);

            float area = 0.5f * crossProduct.magnitude;
            return area;
        }

        private void SerializeMesh(Transform tr, Mesh mesh, BinaryWriter writer) {
            List<Vector3> vertices = mesh.vertices.ToList();
            List<Vector3> normals = mesh.normals.ToList();
            List<Vector2> uv = mesh.uv.ToList();
            List<int> indices = mesh.triangles.ToList();

            List<Vector2> uv1 = new List<Vector2>();
            mesh.GetUVs(1, uv1);

            MeshAssetData meshData = new MeshAssetData();
            meshData.vertices = vertices;
            meshData.normals = normals;
            meshData.uv = uv;
            meshData.indices = indices;
            meshData.uv1 = uv1;
            data.Add(mesh, meshData);

            // mesh type: 120 (PS2, triangle list no index all float4)
            writer.Write((byte)120);

            // engine doesnt support indices, so as disk space is not a problem we save all triangles
            long indicesPosition = writer.BaseStream.Position;
            writer.Write(0);

            // write vertices
            bool hasUV = uv.Count > 0;
            bool hasUV1 = uv1.Count > 0;
            Vector3 scale = tr.lossyScale;
            for (int i = 0; i < indices.Count - 2; i += 3) {
                int i0 = indices[i];
                int i1 = indices[i + 1];
                int i2 = indices[i + 2];
                Vector3 v0 = vertices[i0];
                Vector3 v1 = vertices[i1];
                Vector3 v2 = vertices[i2];

                if (tesselation) {
                    float area = CalculateTriangleArea(
                        Vector3.Scale(v0, scale),
                        Vector3.Scale(v1, scale),
                        Vector3.Scale(v2, scale));

                    if (area > tesselationMinArea) {
                        stats.TesselatedTriangles += 4;

                        // Calculate midpoints of the original triangle
                        Vector3 mid1 = (v0 + v1) / 2;
                        Vector3 mid2 = (v1 + v2) / 2;
                        Vector3 mid3 = (v2 + v0) / 2;

                        Vector3 normal = normals[i0];

                        if (hasUV) {
                            Vector2 texcoord0 = uv[i0];
                            Vector2 texcoord1 = uv[i1];
                            Vector2 texcoord2 = uv[i2];

                            Vector2 midUV1 = (texcoord0 + texcoord1) / 2;
                            Vector2 midUV2 = (texcoord1 + texcoord2) / 2;
                            Vector2 midUV3 = (texcoord2 + texcoord0) / 2;
                            uv.Add(midUV1);
                            uv.Add(midUV2);
                            uv.Add(midUV3);
                        }

                        if (hasUV1) {
                            Vector2 texcoord0 = uv1[i0];
                            Vector2 texcoord1 = uv1[i1];
                            Vector2 texcoord2 = uv1[i2];

                            Vector2 midUV1 = (texcoord0 + texcoord1) / 2;
                            Vector2 midUV2 = (texcoord1 + texcoord2) / 2;
                            Vector2 midUV3 = (texcoord2 + texcoord0) / 2;
                            uv1.Add(midUV1);
                            uv1.Add(midUV2);
                            uv1.Add(midUV3);
                        }

                        int index = vertices.Count;

                        vertices.Add(mid1);
                        vertices.Add(mid2);
                        vertices.Add(mid3);

                        normals.Add(normal);
                        normals.Add(normal);
                        normals.Add(normal);

                        indices.Add(i0);
                        indices.Add(index);
                        indices.Add(index + 2);

                        indices.Add(index);
                        indices.Add(i1);
                        indices.Add(index + 1);

                        indices.Add(index + 2);
                        indices.Add(index + 1);
                        indices.Add(i2);

                        indices.Add(index + 2);
                        indices.Add(index);
                        indices.Add(index + 1);
                    }
                }

                writer.Write(v0);
                writer.Write(1.0f);
                writer.Write(v1);
                writer.Write(1.0f);
                writer.Write(v2);
                writer.Write(1.0f);
            }

            for (int i = 0; i < indices.Count - 2; i += 3) {
                int i0 = indices[i];
                int i1 = indices[i + 1];
                int i2 = indices[i + 2];
                writer.Write(normals[i0]);
                writer.Write(1.0f);
                writer.Write(normals[i1]);
                writer.Write(1.0f);
                writer.Write(normals[i2]);
                writer.Write(1.0f);
            }

            if (uv.Count == 0 || lightmaps == LightmapMode.BruteForce) {
                writer.Write((byte)0);
            } else {
                writer.Write((byte)1);
                for (int i = 0; i < indices.Count - 2; i += 3) {
                    int i0 = indices[i];
                    int i1 = indices[i + 1];
                    int i2 = indices[i + 2];
                    writer.Write(uv[i0]);
                    writer.Write(1.0f);
                    writer.Write(1.0f);
                    writer.Write(uv[i1]);
                    writer.Write(1.0f);
                    writer.Write(1.0f);
                    writer.Write(uv[i2]);
                    writer.Write(1.0f);
                    writer.Write(1.0f);
                }
            }

            long currentPosition = writer.BaseStream.Position;
            writer.BaseStream.Position = indicesPosition;
            writer.Write((uint)indices.Count);
            writer.BaseStream.Position = currentPosition;
        }

        static bool IsPowerOfTwo(double x) {
            return (Math.Log(x, 2) % 1) == 0;  // Check if the logarithm result is an integer
        }

        private void SerializeTexture(string guid, Texture2D tex, BinaryWriter writer, bool isLightmap) {
            writer.Write(guid.ToCharArray());

            bool wasReadable = true;
            string path = AssetDatabase.GetAssetPath(tex);
            TextureImporter importer = null;
            if (isLightmap) {
                writer.Write((ushort)AssetType.Lightmap);
            } else {
                writer.Write((ushort)AssetType.Texture2D);

                importer = (TextureImporter)TextureImporter.GetAtPath(path);
                wasReadable = importer.isReadable;

                if (!wasReadable) {
                    importer.isReadable = true;
                    AssetDatabase.Refresh();
                    AssetDatabase.ImportAsset(path);
                }
            }

            if (IsPowerOfTwo(tex.width)) {
                double power = Math.Log(tex.width, 2);
                if (power > 9) {
                    throw new Exception("Width is bigger than 512");
                }
            } else {
                throw new Exception("Width is not a power of 2");
            }

            if (IsPowerOfTwo(tex.height)) {
                double power = Math.Log(tex.height, 2);
                if (power > 9) {
                    throw new Exception("Height is bigger than 512");
                }
            } else {
                throw new Exception("Height is not a power of 2");
            }

            // serialize texture
            Color32[] colors = tex.GetPixels32();
            writer.Write((ushort)tex.width);
            writer.Write((ushort)tex.height);
            writer.Write((byte)4);
            for (int i = 0; i < colors.Length; i++) {
                writer.Write(colors[i]);
            }

            if (!wasReadable) {
                importer.isReadable = false;
                AssetDatabase.Refresh();
                AssetDatabase.ImportAsset(path);
            }
        }

        private void SerializeMat(string matKey, Material mat, BinaryWriter writer, Dictionary<object, string> assets) {
            // check for textures
            Texture tex = mat.mainTexture;

            string texKey = null;
            if (tex) {
                if (!assets.TryGetValue(tex, out texKey)) {
                    // serialize asset
                    texKey = GUID.Generate().ToString();
                    assets.Add(tex, texKey);

                    SerializeTexture(texKey, (Texture2D)tex, writer, false);
                }
            }

            writer.Write(matKey.ToCharArray());
            writer.Write((ushort)AssetType.Material);

            Color color = mat.color;
            writer.Write((Vector4)color);

            if (tex) {
                writer.Write((byte)1);
                writer.Write(texKey.ToCharArray());
            } else {
                writer.Write((byte)0);
            }
        }

        private void WriteObjectsChild(Transform go, BinaryWriter writer, BinaryWriter assetsWriter, Dictionary<object, string> assets, ProgramData program) {
            writer.Write(go.transform.localPosition);
            writer.Write(go.transform.localScale);
            writer.Write(go.transform.localEulerAngles);

            Component[] comps = go.GetComponents(typeof(Component));
            long compsPos = writer.BaseStream.Position;
            writer.Write((byte)0);

            int totalComps = comps.Length;
            for (int i = 0; i < comps.Length; i++) {
                Component comp = comps[i];

                if (comp is MeshRenderer) {
                    writer.Write((ushort)ComponentType.Mesh);

                    MeshRenderer meshRen = (MeshRenderer)comp;
                    writer.Write(meshRen.enabled);

                    MeshFilter meshFilter = comp.GetComponent<MeshFilter>();
                    Mesh sharedMesh = meshFilter.sharedMesh;

                    string key;
                    if (!assets.TryGetValue(sharedMesh, out key)) {
                        // serialize asset
                        key = GUID.Generate().ToString();
                        assets.Add(sharedMesh, key);

                        assetsWriter.Write(key.ToCharArray());
                        assetsWriter.Write((ushort)AssetType.Mesh);
                        SerializeMesh(go, sharedMesh, assetsWriter);
                    }

                    writer.Write(key.ToCharArray());

                    // lightmap
                    if (lightmaps == LightmapMode.BruteForce) {
                        MeshAssetData meshData = (MeshAssetData)data[sharedMesh];
                        List<int> indices = meshData.indices;
                        if (meshData.uv1.Count == 0) {
                            throw new Exception($"Object ({comp.gameObject.name}) has no lightmaps");
                        }

                        Vector4 lmap = meshRen.lightmapScaleOffset;
                        List<Vector2> newUVs = new List<Vector2>();
                        for (int j = 0; j < indices.Count; j++) {
                            int index = indices[j];
                            Vector2 uv = meshData.uv1[index];
                            uv.x *= lmap.x;
                            uv.y *= lmap.y;
                            uv.x += lmap.z;
                            uv.y += lmap.w;

                            writer.Write(uv);
                            writer.Write(1.0f);
                            writer.Write(1.0f);

                            newUVs.Add(uv);
                        }

                        writer.Write(meshRen.lightmapIndex);
                    } else if (lightmaps == LightmapMode.True) {
                        writer.Write(meshRen.lightmapIndex);
                        Vector4 lmap = meshRen.lightmapScaleOffset;
                        writer.Write(lmap);
                        throw new NotImplementedException();
                    } else {
                        // write material references
                        Material mat = meshRen.sharedMaterial;
                        string matKey;
                        if (!assets.TryGetValue(mat, out matKey)) {
                            // serialize asset
                            matKey = GUID.Generate().ToString();
                            assets.Add(mat, matKey);

                            SerializeMat(matKey, mat, assetsWriter, assets);
                        }

                        writer.Write(matKey.ToCharArray());
                    }

                } else if (comp is Camera) {
                    writer.Write((ushort)ComponentType.Camera);
                    writer.Write(((Camera)comp).enabled);
                } else if (comp is Light) {
                    writer.Write((ushort)ComponentType.Light);

                    Light light = (Light)comp;
                    writer.Write(light.enabled);

                    writer.Write(light.intensity);
                    writer.Write((Vector4)light.color);
                    writer.Write((byte)light.type);

                } else {
                    // check if component is a custom one
                    if (program == null) {
                        totalComps--;
                        continue;
                    }

                    ClassData cl = program.Classes.FirstOrDefault(c => c.Name == comp.GetType().Name);
                    if (cl != null) {
                        if (cl.IsComponent) {
                            writer.Write((ushort)ComponentType.Custom);
                            // write index of class
                            writer.Write((ushort)program.Classes.IndexOf(cl));

                            continue;
                        }
                    }
                }
            }

            long finalPos = writer.BaseStream.Position;
            writer.BaseStream.Position = compsPos;
            writer.Write((byte)totalComps);
            writer.BaseStream.Position = finalPos;

            int counter = 0;
            foreach (Transform tr in go.transform) {
                counter++;
            }

            writer.Write(counter);
            foreach (Transform tr in go.transform) {
                WriteObjectsChild(tr, writer, assetsWriter, assets, program);
            }
        }

        private void PreProcessCode(ProgramData program) {
            AdhocWorkspace workspace = new AdhocWorkspace();
            Solution solution = workspace.CurrentSolution;
            Project project = solution.AddProject("TestProject", "TestAssembly", LanguageNames.CSharp);

            DirectoryInfo folder = new DirectoryInfo(Application.dataPath);
            string fullName = folder.FullName;

            List<FileInfo> files = DirectoryUtil.GetFiles(folder, c => Path.GetExtension(c.Name) != ".cs" || c.FullName.Contains("unity_exporter"));

            List<SyntaxTree> sourceFiles = new List<SyntaxTree>();

            foreach (FileInfo file in files) {
                string testCode = File.ReadAllText(file.FullName);
                Document document = project.AddDocument(file.Name, testCode);
                AsyncUtil.RunSync(async () => { document = await Formatter.FormatAsync(document); });
                SourceText text = null;
                AsyncUtil.RunSync(async () => { text = await document.GetTextAsync(); });

                SyntaxTree tree = CSharpSyntaxTree.ParseText(text);
                sourceFiles.Add(tree);
                CompilationUnitSyntax root = (CompilationUnitSyntax)tree.GetRoot();

                // preprocess all classes without reading functions, so later we can map
                // each class when parsing functions
                foreach (MemberDeclarationSyntax member in root.Members) {
                    Console.WriteLine(member.ToString());

                    if (member is NamespaceDeclarationSyntax) {
                        var nameSpace = (NamespaceDeclarationSyntax)member;
                        foreach (var m in nameSpace.Members) {
                            ExpressionUtil.PreProcessExpression(program, m);
                        }
                    } else {

                    }
                }
            }
        }

        private void WriteClasses(ProgramData program, string outputFolder) {
            for (int i = 0; i < program.Classes.Count; i++) {
                ClassData cl = program.Classes[i];

                if (cl.Declaration == null) {
                    continue;
                }

                string headerPath = Path.Combine(outputFolder, cl.Name + ".h");
                string sourcePath = Path.Combine(outputFolder, cl.Name + ".cpp");
                if (File.Exists(headerPath)) {
                    File.Delete(headerPath);
                }
                if (File.Exists(sourcePath)) {
                    File.Delete(sourcePath);
                }

                using Stream headerStr = File.OpenWrite(headerPath);
                using Stream sourceStr = File.OpenWrite(sourcePath);

                using StreamWriter headerWriter = new StreamWriter(headerStr);
                using StreamWriter sourceWriter = new StreamWriter(sourceStr);

                ExpressionUtil.ProcessDeclaration(cl, program, headerWriter, sourceWriter);

            }
        }

        private void Export() {
            stats = new ExportStats();
            data = new Dictionary<object, AssetData>();

            string fileName = Path.Combine(exportPath, "scene.ps2");
            string assetsName = Path.Combine(exportPath, "assets.ps2");

            Scene scene = SceneManager.GetActiveScene();

            using MemoryStream stream = new MemoryStream();
            using BinaryWriter writer = new BinaryWriter(stream);

            using MemoryStream streamAssets = new MemoryStream();
            using BinaryWriter writerAssets = new BinaryWriter(streamAssets);

            writer.Write(new char[] { 'N', 'U', 'K', 'E' });
            writer.Write((uint)0);
            writer.Write((uint)100);

            writerAssets.Write(new char[] { 'N', 'U', 'K', 'E' });
            writerAssets.Write((uint)1);
            writerAssets.Write((uint)100);

            // write settings
            writer.Write((ushort)platform);
            writer.Write((ushort)lightmaps);

            GameObject[] gos = scene.GetRootGameObjects();
            PreProcessObjs(gos);

            if (lightmaps == LightmapMode.BruteForce) {
                int lightmapCount = LightmapSettings.lightmaps.Length;
                writer.Write((byte)lightmapCount);

                Shader lightMapShader = Shader.Find("Hidden/LMapBaked");

                Material lightMap = new Material(lightMapShader);
                lightMap.SetFloat("_RelFStops", lightmapRelFStops);
                lightMap.SetFloat("_IsLinear", PlayerSettings.colorSpace == ColorSpace.Linear ? 1 : 0);

                for (int i = 0; i < lightmapCount; i++) {
                    List<MeshRenderer> toRender = lightmapMap[i];
                    LightmapData data = LightmapSettings.lightmaps[i];
                    Texture2D texture = data.lightmapColor;

                    lightMap.SetTexture("_LightMapTex", texture);

                    int lightmapSize = Math.Min(lightmapResolution, texture.width);
                    RenderTexture renderTexture = RenderTexture.GetTemporary(lightmapSize, lightmapSize, 0, RenderTextureFormat.ARGB32);
                    Graphics.SetRenderTarget(renderTexture);
                    GL.Clear(true, true, new Color(0, 0, 0, 1.0f)); // clear to transparent

                    for (int k = 0; k < toRender.Count; k++) {
                        MeshRenderer renderer = toRender[k];
                        MeshFilter filter = renderer.GetComponent<MeshFilter>();

                        Mesh mesh = filter.sharedMesh;
                        Transform trans = renderer.transform;
                        Matrix4x4 world = Matrix4x4.TRS(trans.position, trans.rotation, trans.lossyScale);

                        Material[] mats = renderer.sharedMaterials;
                        for (int j = 0; j < mats.Length; j++) {
                            Material mat = mats[j];

                            // clear to default
                            lightMap.SetTexture("_MainTex", EditorGUIUtility.whiteTexture);
                            lightMap.SetColor("_Color", Color.white);

                            // uvs
                            lightMap.SetVector("_LightMapUV", renderer.lightmapScaleOffset);

                            Shader shader = mat.shader;
                            int props = ShaderUtil.GetPropertyCount(shader);
                            for (int l = 0; l < props; l++) {
                                string name = ShaderUtil.GetPropertyName(shader, l);

                                ShaderUtil.ShaderPropertyType propType = ShaderUtil.GetPropertyType(shader, l);
                                if (propType == ShaderUtil.ShaderPropertyType.TexEnv) {
                                    if (ExporterGlobals.SemanticsMainTex.Contains(name.ToLower())) {
                                        // main texture texture
                                        Texture matTex = mat.GetTexture(name);
                                        if (matTex) {
                                            lightMap.SetTexture("_MainTex", matTex);
                                        }
                                    }
                                } else if (propType == ShaderUtil.ShaderPropertyType.Color) {
                                    if (ExporterGlobals.SemanticsColor.Contains(name.ToLower())) {
                                        lightMap.SetColor("_Color", mat.GetColor(name));
                                    }
                                }
                            }

                            lightMap.SetPass(0);
                            Graphics.DrawMeshNow(mesh, world, j);
                        }
                    }

                    // This is the only way to access data from a RenderTexture
                    Texture2D tex = new Texture2D(renderTexture.width, renderTexture.height, TextureFormat.ARGB32, false, false);
                    tex.ReadPixels(new Rect(0, 0, tex.width, tex.height), 0, 0);

                    Graphics.SetRenderTarget(null);
                    RenderTexture.ReleaseTemporary(renderTexture);

                    SerializeTexture(GUID.Generate().ToString(), tex, writer, true);
                }
            }

            ProgramData program = null;

            if (exportCode) {
                program = new ProgramData();
                program.Classes.Add(ExpressionUtil.MonoBehaviourData);
                program.Classes.Add(ExpressionUtil.InputData);
                program.Classes.Add(ExpressionUtil.TimeData);
                program.Classes.Add(ExpressionUtil.Vector2Data);
                program.Classes.Add(ExpressionUtil.Vector3Data);
                program.Classes.Add(ExpressionUtil.Vector4Data);

                PreProcessCode(program);
                ExpressionUtil.ProcessProgram(program);
                WriteClasses(program, Path.Combine(codePath, "game"));
            }

            Dictionary<object, string> assets = new Dictionary<object, string>();
            WriteObjects(gos, writer, writerAssets, assets, program);

            // write final data
            writer.Flush();
            writerAssets.Flush();

            if (File.Exists(fileName)) {
                File.Delete(fileName);
            }
            if (File.Exists(assetsName)) {
                File.Delete(assetsName);
            }

            using (Stream fileStr = File.OpenWrite(fileName)) {
                stream.Position = 0;
                stream.CopyTo(fileStr);
            }

            using (Stream fileStr = File.OpenWrite(assetsName)) {
                streamAssets.Position = 0;
                streamAssets.CopyTo(fileStr);
            }

            if (tesselation) {
                Debug.Log(stats.TesselatedTriangles + " New Triangles");
            }
        }
    }
}