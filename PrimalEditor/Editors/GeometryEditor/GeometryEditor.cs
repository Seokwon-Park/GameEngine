using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.IO.Packaging;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using PrimalEditor.Content;

namespace PrimalEditor.Editors
{
    // NOTE: the purpose of this class is to enable viewing 3D geometry in WPF while
    //		 we don't have a graphics renderer in the game engine. When we have a renderer,
    //		 this class and the WPF viewer will become obsolete.
    class MeshRendererVertexData : ViewModelBase
    {
        private bool _isHighlighted;
        public bool IsHighlighted
        {
            get => _isHighlighted;
            set
            {
                if (_isHighlighted != value)
                {
                    _isHighlighted = value;
                    OnPropertyChanged(nameof(IsHighlighted));
                    OnPropertyChanged(nameof(Diffuse));
                }
            }
        }

        private bool _isIsolated;
        public bool IsIsolated
        {
            get => _isIsolated;
            set
            {
                if (_isIsolated != value)
                {
                    _isIsolated = value;
                    OnPropertyChanged(nameof(IsIsolated));
                }
            }
        }

        private Brush _specular = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#ff111111"));
        public Brush Specular
        {
            get => _specular;
            set
            {
                if (_specular != value)
                {
                    _specular = value;
                    OnPropertyChanged(nameof(Specular));
                }
            }
        }

        private Brush _diffuse = Brushes.White;
        public Brush Diffuse
        {
            get => _isHighlighted ? Brushes.Orange : _diffuse;
            set
            {
                if (_diffuse != value)
                {
                    _diffuse = value;
                    OnPropertyChanged(nameof(Diffuse));
                }
            }
        }
        public string Name { get; set; }
        public Point3DCollection Positions { get; } = new Point3DCollection();
        public Vector3DCollection Normals { get; } = new Vector3DCollection();
        public PointCollection UVs { get; } = new PointCollection();
        public Int32Collection Indices { get; } = new Int32Collection();
    }

    // NOTE: the purpose of this class is to enable viewing 3D geometry in WPF while
    //		 we don't have a graphics renderer in the game engine. When we have a renderer,
    //		 this class and the WPF viewer will become obsolete.
    class MeshRenderer : ViewModelBase
    {
        public ObservableCollection<MeshRendererVertexData> Meshes { get; } = new ObservableCollection<MeshRendererVertexData>();

        // TODO: 나중에 left handed 적용
        //right-handed
        private Vector3D _cameraDirection = new Vector3D(0, 0, -10);
        public Vector3D CameraDirection
        {
            get => _cameraDirection;
            set
            {
                if (_cameraDirection != value)
                {
                    _cameraDirection = value;
                    OnPropertyChanged(nameof(CameraDirection));
                }
            }
        }

        private Point3D _cameraPosition = new Point3D(0, 0, 10);
        public Point3D CameraPosition
        {
            get => _cameraPosition;
            set
            {
                if (_cameraPosition!= value)
                {
                    _cameraPosition= value;
                    CameraDirection = new Vector3D(-value.X, -value.Y, -value.Z);
                    OnPropertyChanged(nameof(OffsetCameraPosition));
                    OnPropertyChanged(nameof(CameraPosition));
                }
            }
        }

        private Point3D _cameraTarget = new Point3D(0, 0, 0);
        public Point3D CameraTarget
        {
            get => _cameraTarget;
            set
            {
                if (_cameraTarget!= value)
                {
                    _cameraTarget= value;
                    OnPropertyChanged(nameof(OffsetCameraPosition));
                    OnPropertyChanged(nameof(CameraTarget));
                }
            }
        }

        public Point3D OffsetCameraPosition =>
            new Point3D(CameraPosition.X + CameraTarget.X, CameraPosition.Y + CameraTarget.Y, CameraPosition.Z + CameraTarget.Z);

        private Color _keyLight = (Color)ColorConverter.ConvertFromString("#ffffffff");
        public Color KeyLight
        {
            get => _keyLight;
            set
            {
                if (_keyLight != value)
                {
                    _keyLight = value;
                    OnPropertyChanged(nameof(KeyLight));
                }
            }
        }

        private Color _skyLight = (Color)ColorConverter.ConvertFromString("#ff111b30");
        public Color SkyLight
        {
            get => _skyLight;
            set
            {
                if (_skyLight != value)
                {
                    _skyLight = value;
                    OnPropertyChanged(nameof(SkyLight));
                }
            }
        }

        private Color _groundLight= (Color)ColorConverter.ConvertFromString("#ff3f2f1e");
        public Color GroundLight
        {
            get => _groundLight;
            set
            {
                if (_groundLight != value)
                {
                    _groundLight = value;
                    OnPropertyChanged(nameof(GroundLight));
                }
            }
        }

        private Color _ambientLight = (Color)ColorConverter.ConvertFromString("#ff3b3b3b");
        public Color AmbientLight
        {
            get => _ambientLight;
            set
            {
                if (_ambientLight != value)
                {
                    _ambientLight = value;
                    OnPropertyChanged(nameof(AmbientLight));
                }
            }
        }

        public MeshRenderer(MeshLOD lod, MeshRenderer old)
        {
            Debug.Assert(lod?.Meshes.Any() == true);
            // Calculate vertex size minus the position and normal vectors.

            // 카메라 포지션과 타겟을 적절하게 설정하기 위해서 , 우리는 렌더링하는 이 오브젝트가 얼마나 큰지 알아내야 함.
            // 그러기 위해서는 경계상자를 알아야 한다.
            double minX, minY, minZ; minX = minY = minZ = double.MaxValue;
            double maxX, maxY, maxZ; maxX = maxY = maxZ = double.MinValue;
            Vector3D avgNormal = new Vector3D();

            // packed normals -> unpacked
            var intervals = 2.0f / ((1 << 16) - 1);

            foreach (var mesh in lod.Meshes)
            {
                var vertexData = new MeshRendererVertexData() { Name = mesh.Name };

                using (var reader = new BinaryReader(new MemoryStream(mesh.Positions)))
                    for (int i = 0; i < mesh.VertexCount; ++i)
                    {
                        // Read Positions
                        var posX = reader.ReadSingle();
                        var posY = reader.ReadSingle();
                        var posZ = reader.ReadSingle();
                        vertexData.Positions.Add(new Point3D(posX, posY, posZ));

                        // Adjust the bounding box:
                        minX = Math.Min(minX, posX); maxX = Math.Max(maxX, posX);
                        minY = Math.Min(minY, posY); maxY = Math.Max(maxY, posY);
                        minZ = Math.Min(minZ, posZ); maxZ = Math.Max(maxZ, posZ);
                    }

                if (mesh.ElementsType.HasFlag(ElementsType.Normals))
                {
                    var tSpaceOffset = 0;
                    if (mesh.ElementsType.HasFlag(ElementsType.Joints)) tSpaceOffset = sizeof(short) * 4; // skip joint indices.
                    // Read normals
                    using (var reader = new BinaryReader(new MemoryStream(mesh.Elements)))
                        for (int i = 0; i < mesh.VertexCount; ++i)
                        {
                            var signs = (reader.ReadUInt32() >> 24) & 0x000000ff;
                            reader.BaseStream.Position += tSpaceOffset;
                            // Reade normals
                            var normX = reader.ReadUInt16() * intervals - 1.0f;
                            var normY = reader.ReadUInt16() * intervals - 1.0f;
                            var normZ = Math.Sqrt(Math.Clamp(1f - (normX * normX + normY * normY), 0f, 1f)) * ((signs & 0x2) - 1f);
                            var normal = new Vector3D(normX, normY, normZ);
                            normal.Normalize();
                            vertexData.Normals.Add(normal);
                            avgNormal += normal;

                            //Read UVs (skip tangent and joint data)
                            if (mesh.ElementsType.HasFlag(ElementsType.TSpace))
                            {
                                reader.BaseStream.Position += sizeof(short) * 2; // skip tangents
                                var u = reader.ReadSingle();
                                var v = reader.ReadSingle();
                                vertexData.UVs.Add(new Point(u, v));
                            }

                            if (mesh.ElementsType.HasFlag(ElementsType.Joints) && mesh.ElementsType.HasFlag(ElementsType.Colors))
                            {
                                reader.BaseStream.Position += 4; // skip colors.
                            }
                        }
                }

                using (var reader = new BinaryReader(new MemoryStream(mesh.Indices)))
                {
                    if (mesh.IndexSize == sizeof(short))
                        for (int i = 0; i < mesh.IndexCount; ++i) vertexData.Indices.Add(reader.ReadUInt16());
                    else
                        // ReadUInt32가 아닌 이유: vertices와 index values를 많이 가지고 있다면 16 비트 정수형에 맞는 것보다
                        // 커지게 될 것이고 그 경우 signed integer을 사용해야 한다. 왜냐하면 그렇게 하지 않는 경우
                        // 값들이 Int32Collection에서 음수로 번역되고, 너가 큰 unsigned 값을 이 콜렉션에 넣게 되면
                        // negative로 들어갈 것이고 명확하게 동작하지 않는다.

                        // 이해함 -> 쉽게 말해서 Int32Collection은 Int32의 최대 최소 범위이기 때문에
                        // UInt32로 읽게 되면 버퍼 오버플로우가 발생합니다. 그래서 Int32로 읽으므로 최대 정점수는 int32의 최댓값
                        for (int i = 0; i < mesh.IndexCount; ++i) vertexData.Indices.Add(reader.ReadInt32());

                    vertexData.Positions.Freeze();
                    vertexData.Normals.Freeze();
                    vertexData.UVs.Freeze();
                    vertexData.Indices.Freeze();
                    Meshes.Add(vertexData);
                }
            }

            // set caemera target and position
            if(old!=null)
            {
                CameraTarget = old.CameraTarget;
                CameraPosition = old.CameraPosition;

                // NOTE: this is only for primitive meshes with multiple LODs,
                //       because tehy're displayed with textures:
                foreach(var mesh in old.Meshes)
                {
                    mesh.IsHighlighted = false;
                }

                foreach(var mesh in Meshes)
                {
                    mesh.Diffuse = old.Meshes.First().Diffuse;
                }
            }
            else
            {
                var width = maxX - minX;
                var height = maxY - minY;
                var depth = maxZ - minZ;
                var radius = new Vector3D(height, width, depth).Length * 1.2;
                if(avgNormal.Length > 0.8)
                {
                    avgNormal.Normalize();
                    avgNormal *= radius;
                    CameraPosition = new Point3D(avgNormal.X, avgNormal.Y, avgNormal.Z);
                }
                else
                {
                    CameraPosition = new Point3D(width, height * 0.5, radius);
                }
                //타겟의 중심 구하기
                CameraTarget = new Point3D(minX + width * 0.5, minY + height * 0.5, minZ + depth * 0.5);
            }            
        }
    }

    class GeometryEditor : ViewModelBase, IAssetEditor
    {
        public Content.Asset Asset => Geometry;

        private Content.Geometry _geometry;
        public Content.Geometry Geometry
        {
            get => _geometry;
            set
            {
                if (_geometry != value)
                {
                    _geometry = value;
                    OnPropertyChanged(nameof(Geometry));
                }
            }
        }

        private MeshRenderer _meshRenderer;
        public MeshRenderer MeshRenderer
        {
            get => _meshRenderer;
            set
            {
                if (_meshRenderer != value)
                {
                    _meshRenderer = value;
                    OnPropertyChanged(nameof(MeshRenderer));
                    var lods = Geometry.GetLODGroup().LODs;
                    MaxLODIndex = (lods.Count > 0) ? lods.Count - 1 : 0;
                    OnPropertyChanged(nameof(MaxLODIndex));
                    if(lods.Count>1)
                    {
                        MeshRenderer.PropertyChanged += (s, e) =>
                        {
                            if (e.PropertyName == nameof(MeshRenderer.OffsetCameraPosition) && AutoLOD) ComputeLOD(lods);
                        };

                        ComputeLOD(lods);
                    }
                }
            }
        }

        private bool _autoLOD;
        public bool AutoLOD
        {
            get => _autoLOD;
            set
            {
                if (_autoLOD != value)
                {
                    _autoLOD = value;
                    OnPropertyChanged(nameof(AutoLOD));
                }
            }
        }

        public int MaxLODIndex{ get; private set; }

        private int _lodIndex;
        public int LODIndex
        {
            get => _lodIndex;
            set
            {
                var lods = Geometry.GetLODGroup().LODs;
                value = Math.Clamp(value, 0, lods.Count);
                if (_lodIndex != value)
                {
                    _lodIndex = value;
                    OnPropertyChanged(nameof(LODIndex));
                    MeshRenderer = new MeshRenderer(lods[value], MeshRenderer);
                }
            }
        }

        private void ComputeLOD(IList<MeshLOD> lods)
        {
            if (!AutoLOD) return;

            var p = MeshRenderer.OffsetCameraPosition;
            var distance = new Vector3D(p.X, p.Y, p.Z).Length;
            for(int i = MaxLODIndex; i>=0; --i)
            {
                if (lods[i].LodThreshold < distance)
                {
                    LODIndex = i;
                    break;
                }
            }
        }

        public void SetAsset(Asset asset)
        {
            Debug.Assert(asset is Content.Geometry);
            if (asset is Content.Geometry geometry)
            {
                Geometry = geometry;
                var numLods = geometry.GetLODGroup().LODs.Count;
                if(LODIndex >= numLods)
                {
                    LODIndex = numLods - 1;
                }
                else
                {
                    MeshRenderer = new MeshRenderer(Geometry.GetLODGroup().LODs[0], MeshRenderer);
                }
            }
        }

        public async void SetAsset(AssetInfo info)
        {
            try
            {
                Debug.Assert(info != null && File.Exists(info.FullPath));
                var geometry = new Content.Geometry();
                await Task.Run(() =>
                {
                    geometry.Load(info.FullPath);
                });

                SetAsset(geometry);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
        }
    }
}
