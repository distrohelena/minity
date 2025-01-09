#if UNITY_EDITOR

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UObject = UnityEngine.Object;

namespace ps2unity {
    /// <summary>
    /// Global values for JanusVR
    /// </summary>
    public static class ExporterGlobals {

        public const decimal Version = 1.00M;

        /// <summary>
        /// Decimal cases used by default when exporting position/scale/rotation values
        /// </summary>
        public static int DecimalCasesForTransforms = 4;

        /// <summary>
        /// Decimal cases used by default when exporting lightmaps
        /// </summary>
        public static int DecimalCasesForLightmaps = 6;

        /// <summary>
        /// Lower case values that the exporter will consider for being the Main Texture on a shader
        /// </summary>
        public static readonly string[] SemanticsMainTex = new string[]
        {
            "_maintex"
        };

        /// <summary>
        /// Lower case values that the exporter will consider for being the Tiling
        /// </summary>
        public static readonly string[] SemanticsTiling = new string[]
        {
            "_maintex_st"
        };

        /// <summary>
        /// Lower case values that the exporter will consider for being the Color off a shader
        /// </summary>
        public static readonly string[] SemanticsColor = new string[]
        {
            "_color"
        };

        /// <summary>
        /// Lower case values that the exporter will consider for the shader using transparent textures
        /// </summary>
        public static readonly string[] SemanticsTransparent = new string[]
        {
            "transparent"
        };

        /// <summary>
        /// The semantic names for all the skybox 6-sided faces
        /// </summary>
        public static readonly string[] SkyboxTexNames = new string[]
        {
            "_FrontTex", "_BackTex", "_LeftTex", "_RightTex", "_UpTex", "_DownTex"
        };
    }
}
#endif