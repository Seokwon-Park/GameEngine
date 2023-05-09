﻿using PrimalEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;

namespace PrimalEditor.Content
{ 
    enum PrimitiveMeshType
    {
        //enum primitive_mesh_type : u32
        //{
        //    plane,
        //    cube,
        //    uv_sphere,
        //    ico_sphere,
        //    cylinder,
        //    capsule,

        //    count
        //};

        Plane,
        Cube,
        UvSphere,
        IcoSphere,
        Cylinder,
        Capsule
    }

    class Mesh : ViewModelBase
    {
        private int _vertexSize;
        public int VertexSize
        {
            get => _vertexSize;
            set
            {
                if (_vertexSize != value)
                {
                    _vertexSize = value;
                    OnPropertyChanged(nameof(VertexSize));
                }
            }
        }

        private int _vertexCount;
        public int VertexCount
        {
            get => _vertexCount;
            set
            {
                if (_vertexCount != value)
                {
                    _vertexCount = value;
                    OnPropertyChanged(nameof(VertexCount));
                }
            }
        }

        private int _indexSize;
        public int IndexSize
        {
            get => _indexSize;
            set
            {
                if (_indexSize != value)
                {
                    _indexSize = value;
                    OnPropertyChanged(nameof(IndexSize));
                }
            }
        }

        private int _indexCount;
        public int IndexCount
        {
            get => _indexCount;
            set
            {
                if (_indexCount != value)
                {
                    _indexCount = value;
                    OnPropertyChanged(nameof(IndexCount));
                }
            }
        }

        public byte[] Vertices { get; set; }
        public byte[] Indices { get; set; }
    }

    class MeshLOD: ViewModelBase
    {
        private string _name;
        public string Name
        {
            get => _name;
            set
            {
                if (_name != value)
                {
                    _name = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }

        private float _lodThreshold;
        public float LodThreshold
        {
            get => _lodThreshold;
            set
            {
                if (_lodThreshold != value)
                {
                    _lodThreshold = value;
                    OnPropertyChanged(nameof(LodThreshold));
                }
            }
        }

        public ObservableCollection<Mesh> Meshes { get; } = new ObservableCollection<Mesh>();
    }

    class LODGroup : ViewModelBase
    {
        private string _name;
        public string Name
        {
            get => _name;
            set
            {
                if (_name != value)
                {
                    _name = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }

        public ObservableCollection<MeshLOD> LODs { get; } = new ObservableCollection<MeshLOD>();
    }

    class GeometryImportSettings : ViewModelBase
    {

        private bool _calculateNormals;
        public bool CalculateNormals
        {
            get => _calculateNormals;
            set
            {
                if (_calculateNormals != value)
                {
                    _calculateNormals = value;
                    OnPropertyChanged(nameof(CalculateNormals));
                }
            }
        }

        private bool _calculateTangents;
        public bool CalculateTangents
        {
            get => _calculateTangents;
            set
            {
                if (_calculateTangents != value)
                {
                    _calculateTangents = value;
                    OnPropertyChanged(nameof(CalculateTangents));
                }
            }
        }

        private float _smoothingAngle;
        public float SmoothingAngle
        {
            get => _smoothingAngle;
            set
            {
                if (_smoothingAngle != value)
                {
                    _smoothingAngle = value;
                    OnPropertyChanged(nameof(SmoothingAngle));
                }
            }
        }

        private bool _reverseHandedness;
        public bool ReverseHandedness
        {
            get => _reverseHandedness;
            set
            {
                if (_reverseHandedness != value)
                {
                    _reverseHandedness = value;
                    OnPropertyChanged(nameof(ReverseHandedness));
                }
            }
        }

        private bool _importEmbeddedTextures;
        public bool ImportEmbeddedTextures
        {
            get => _importEmbeddedTextures;
            set
            {
                if (_importEmbeddedTextures != value)
                {
                    _importEmbeddedTextures = value;
                    OnPropertyChanged(nameof(ImportEmbeddedTextures));
                }
            }
        }

        private bool _importAnimations;
        public bool ImportAnimations
        {
            get => _importAnimations;
            set
            {
                if (_importAnimations!= value)
                {
                    _importAnimations= value;
                    OnPropertyChanged(nameof(ImportAnimations));
                }
            }
        }

        public GeometryImportSettings()
        {
            CalculateNormals = false;
            CalculateNormals = false;
            SmoothingAngle = 178f;
            ReverseHandedness = false;
            ImportEmbeddedTextures = true;
            ImportAnimations = true;
        }
    }


    class Geometry : Asset
    {
        private readonly List<LODGroup> _lodGroups = new List<LODGroup>();

        public GeometryImportSettings ImportSettings { get; } = new GeometryImportSettings ();

        public LODGroup GetLODGroup(int lodGroup = 0)
        {
            Debug.Assert(lodGroup >= 0 && lodGroup < _lodGroups.Count);
            return _lodGroups.Any() ? _lodGroups[lodGroup] : null;
        }


        public void FromRawData(byte[] data)
        {
            Debug.Assert(data?.Length > 0);
            _lodGroups.Clear();

            using var reader = new BinaryReader(new MemoryStream(data));
            // skip scene name string
            var s = reader.ReadInt32(); // u32
            reader.BaseStream.Position += s;
            // get number of LODs
            var numLODGroups = reader.ReadInt32();
            Debug.Assert(numLODGroups > 0);

            for(int i = 0; i< numLODGroups; ++i)
            {
                //get LOD group's name
                s = reader.ReadInt32();
                string lodGroupName;
                if(s > 0)
                {
                    var nameBytes = reader.ReadBytes(s);
                    lodGroupName = Encoding.UTF8.GetString(nameBytes);
                }
                else
                {
                    lodGroupName = $"lod_{ContentHelper.GetRandomString()}";
                }

                // get number of meshes in this LOD group
                var numMeshes = reader.ReadInt32(); 
                Debug.Assert(numMeshes > 0);
                List<MeshLOD> lods = ReadMeshLODs(numMeshes, reader);

                var lodGroup = new LODGroup() { Name = lodGroupName };
                lods.ForEach(l => lodGroup.LODs.Add(l));

                _lodGroups.Add(lodGroup);
            }
        }

        private static List<MeshLOD> ReadMeshLODs(int numMeshes, BinaryReader reader)
        {
            var lodIds = new List<int>();
            var lodList = new List<MeshLOD>();
            for(int i =0; i< numMeshes; ++i)
            {
                ReadMeshes(reader,lodIds, lodList);
            }

            return lodList;
        }

        private static void ReadMeshes(BinaryReader reader, List<int> lodIds, List<MeshLOD> lodList)
        {
            //get mesh's name
            var s = reader.ReadInt32();
            string meshName;
            if(s>0)
            {
                var nameBytes = reader.ReadBytes(s);
                meshName = Encoding.UTF8.GetString(nameBytes);
            }
            else
            {
                meshName = $"mesh_{ContentHelper.GetRandomString()}";
            }

            var mesh = new Mesh();

            var lodId = reader.ReadInt32();
            mesh.VertexSize = reader.ReadInt32();
            mesh.VertexCount = reader.ReadInt32();
            mesh.IndexSize= reader.ReadInt32();
            mesh.IndexCount = reader.ReadInt32();
            var lodThreshold = reader.ReadSingle();

            var vertexBufferSize = mesh.VertexSize * mesh.VertexCount;
            var indexBufferSize = mesh.IndexSize * mesh.IndexCount;

            mesh.Vertices = reader.ReadBytes(vertexBufferSize);
            mesh.Indices = reader.ReadBytes(indexBufferSize);

            MeshLOD lod;
            if(ID.IsValid(lodId) && lodIds.Contains(lodId))
            {
                lod = lodList[lodIds.IndexOf(lodId)];
                Debug.Assert(lod != null);                   
            }
            else
            {
                lodIds.Add(lodId);
                lod = new MeshLOD() { Name = meshName, LodThreshold = lodThreshold };
                lodList.Add(lod);
            }
            lod.Meshes.Add(mesh);
        }

        public override IEnumerable<string> Save(string file)
        {
            Debug.Assert(_lodGroups.Any());
            var savedFiles = new List<string>();
            if(!_lodGroups.Any()) return savedFiles;

            var path = Path.GetDirectoryName(file) + Path.DirectorySeparatorChar;
            var fileName = Path.GetFileNameWithoutExtension(file);

            try
            {
                foreach(var lodGroup in _lodGroups)
                {
                    Debug.Assert(lodGroup.LODs.Any());
                    // Use the name of most detailed LOD for file name
                    var meshFileName =ContentHelper.SanitizeFileName( path + fileName + "_" + lodGroup.LODs[0].Name + AssetFileExtension);
                    // NOTE: we have to make a diff id for each new asset file.
                    Guid = Guid.NewGuid();
                    byte[] data = null;
                    using (var writer = new BinaryWriter(new MemoryStream()))
                    {
                        writer.Write(lodGroup.Name);
                        writer.Write(lodGroup.LODs.Count);
                        var hashes = new List<byte>();
                        foreach(var lod in lodGroup.LODs)
                        {
                            LODToBinary(lod, writer, out var hash);
                            hashes.AddRange(hash);
                        }

                        Hash = ContentHelper.ComputeHash(hashes.ToArray());
                        data = (writer.BaseStream as MemoryStream).ToArray();
                        Icon = GenerateIcon(lodGroup.LODs[0]);
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                Logger.Log(MessageType.Error, $"Failed to save geometry to {file}");
            }

            return savedFiles;
        }

        private void LODToBinary(MeshLOD lod, BinaryWriter writer, out byte[] hash)
        {
            writer.Write(lod.Name);
            writer.Write(lod.LodThreshold);
            writer.Write(lod.Meshes.Count);

            var meshDataBegin = writer.BaseStream.Position;

            foreach(var mesh in lod.Meshes)
            {
                writer.Write(mesh.VertexSize);
                writer.Write(mesh.VertexCount);
                writer.Write(mesh.IndexSize);
                writer.Write(mesh.IndexCount);
                writer.Write(mesh.Vertices);
                writer.Write(mesh.Indices);
            }

            var meshDataSize = writer.BaseStream.Position - meshDataBegin;
            Debug.Assert(meshDataSize > 0);
            var buffer = (writer.BaseStream as MemoryStream).ToArray();
            hash = ContentHelper.ComputeHash(buffer, (int)meshDataBegin, (int)meshDataSize);
        }

        public Geometry() : base(AssetType.Mesh)
        {

        }
    }
}
