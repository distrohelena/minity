#if UNITY_EDITOR
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEngine;

namespace ps2unity {
        public static class ExporterResources {
        private static Texture2D tempRenderTexture;
        public static Texture2D TempRenderTexture {
            get {
                if (!tempRenderTexture) {
                    tempRenderTexture = new Texture2D(256, 256);
                }
                return tempRenderTexture;
            }
        }

        private static Material exposureMaterial;
        public static Material ExposureMaterial {
            get {
                if (!exposureMaterial) {
                    Shader exposureShader = Shader.Find("Hidden/ExposureShader");
                    exposureMaterial = new Material(exposureShader);
                    exposureMaterial.SetPass(0);
                    exposureMaterial.SetFloat("_IsLinear", PlayerSettings.colorSpace == ColorSpace.Linear ? 1 : 0);
                }
                return exposureMaterial;
            }
        }


        private static Mesh planeMesh;
        public static Mesh PlaneMesh {
            get {
                if (!planeMesh) {
                    planeMesh = new Mesh();
                    planeMesh.name = "Janus Plane";
                    planeMesh.hideFlags = HideFlags.HideAndDontSave;

                    float width = 1;
                    float height = 1;
                    float halfWidth = width / 2.0f;
                    float halfHeight = height / 2.0f;

                    Vector3[] vertices = new Vector3[]
                    {
                        new Vector3(-halfWidth, -halfHeight, 0),
                        new Vector3(halfWidth, -halfHeight, 0),
                        new Vector3(halfWidth, halfHeight, 0),
                        new Vector3(-halfWidth, halfHeight, 0)
                    };

                    Vector2[] uv = new Vector2[]
                    {
                        new Vector2 (0, 0),
                        new Vector2 (1, 0),
                        new Vector2 (1, 1),
                        new Vector2 (0, 1)
                    };

                    int[] triangles = new int[]
                    {
                        // plane
                        0, 1, 2, 0, 2, 3,
                        2, 1, 0, 3, 2, 0
                    };

                    planeMesh.vertices = vertices;
                    planeMesh.triangles = triangles;
                    planeMesh.uv = uv;

                    planeMesh.RecalculateNormals();

                    planeMesh.UploadMeshData(true);
                }

                return planeMesh;
            }
        }

        private static int cylinderBasePrecision = 32;
        public static int CylinderBasePrecision {
            get { return cylinderBasePrecision; }
            set {
                cylinderBasePrecision = Mathf.Clamp(value, 3, 128);
            }
        }

        private static Mesh cylinderBaseMesh;
        public static Mesh CylinderBaseMesh {
            get {
                if (!cylinderBaseMesh) {
                    cylinderBaseMesh = new Mesh();
                    cylinderBaseMesh.name = "Janus Circular";
                    cylinderBaseMesh.hideFlags = HideFlags.HideAndDontSave;

                    float fullCircumference = Mathf.PI * 2;
                    float step = fullCircumference / (float)cylinderBasePrecision;

                    Vector3[] vertices = new Vector3[(cylinderBasePrecision * 2) + 1];
                    int vIndex = 1;
                    int[] triangles = new int[cylinderBasePrecision * 3];

                    for (int i = 0; i < cylinderBasePrecision; i++) {
                        float angle = step * i;
                        float cosA = (float)Math.Cos(angle);
                        float sinA = (float)Math.Sin(angle);

                        int index = i * 3;
                        triangles[index] = 0;
                        triangles[index + 1] = vIndex;
                        triangles[index + 2] = vIndex + 1;

                        vertices[vIndex++] = new Vector3(cosA / 2.0f, sinA / 2.0f, 0);
                    }

                    triangles[triangles.Length - 1] = triangles[1]; // full circle

                    cylinderBaseMesh.vertices = vertices;
                    cylinderBaseMesh.triangles = triangles;

                    cylinderBaseMesh.RecalculateNormals();

                    cylinderBaseMesh.UploadMeshData(true);
                }

                return cylinderBaseMesh;
            }
        }
    }
}
#endif