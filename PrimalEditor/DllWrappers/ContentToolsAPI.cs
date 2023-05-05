﻿using PrimalEditor.ContentToolsAPIStructs;
using PrimalEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace PrimalEditor.ContentToolsAPIStructs
{
    [StructLayout(LayoutKind.Sequential)]
    public class GeometryImportSettings
    {
        public float SmoothingAngle = 178f;
        public byte CalculateNormals = 0;
        public byte CalculateTangents = 1;
        public byte ReverseHandedness = 0;
        public byte ImportEmbededTextures = 1;
        public byte ImportAnimations = 1;
    }

    [StructLayout(LayoutKind.Sequential)]
    class SceneData : IDisposable
    {
        public IntPtr Data;
        public int DataSize;
        public GeometryImportSettings ImportSettings = new GeometryImportSettings();

        public void Dispose()
        {
            Marshal.FreeCoTaskMem(Data);
            //
            GC.SuppressFinalize(this);
        }

        ~SceneData()
        {
            Dispose();
        }
    }


    [StructLayout(LayoutKind.Sequential)]
    class PrimitiveInitInfo
    {
        //struct primitive_init_info
        //{
        //	primitive_mesh_type type;
        //	u32 segments[3]{ 1,1,1 }; // x, y, z
        //	math::Vector3 size{ 1,1,1 };
        //	u32 lod{ 0 };
        //};

        public Content.PrimitiveMeshType Type;
        public int SegmentX = 1;
        public int SegmentY = 1;
        public int SegmentZ = 1;
        public Vector3 Size = new Vector3(1f);
        public int LOD = 0;
    }


}


namespace PrimalEditor.DllWrappers
{
    static class ContentToolsAPI
    {
        private const string _toolsDll = "ContentTools.dll";

        [DllImport(_toolsDll)]
        private static extern void CreatePrimitiveMesh([In, Out] SceneData data, PrimitiveInitInfo info);
        public static void CreatePrimitiveMesh(Content.Geometry geometry, PrimitiveInitInfo info)
        {
            Debug.Assert(geometry != null);
            using var sceneData = new SceneData();
            try
            {
                CreatePrimitiveMesh(sceneData, info);
                Debug.Assert(sceneData.Data != IntPtr.Zero && sceneData.DataSize > 0);
                var data = new byte[sceneData.DataSize];
                Marshal.Copy(sceneData.Data, data, 0, sceneData.DataSize);
                geometry.FromRawData(data);
            }
            catch (Exception ex)
            {
                Logger.Log(MessageType.Error, $"failed to create {info.Type} primitive mesh.");
                Debug.WriteLine(ex.Message);
            }
        }
    }
}