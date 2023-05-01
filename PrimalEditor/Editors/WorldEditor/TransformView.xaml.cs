using PrimalEditor.Components;
using PrimalEditor.GameProject;
using PrimalEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace PrimalEditor.Editors
{
    /// <summary>
    /// Interaction logic for TransformView.xaml
    /// </summary>
    public partial class TransformView : UserControl
    {
        private Action _undoAction = null;
        private bool _propertyChanged = false;

        public TransformView()
        {
            InitializeComponent();
            Loaded += OnTransformViewLoaded;
        }

        private void OnTransformViewLoaded(object sender, RoutedEventArgs e)
        {
            Loaded -= OnTransformViewLoaded;
            (DataContext as MSTransform).PropertyChanged += (s, e) => _propertyChanged = true;
        }

        private Action GetAction(Func<Transform, (Transform transform, Vector3)> selector,
            Action<(Transform transform, Vector3)> forEachAction)
        {
            // DataContext 에서 MSTransform(multiselect)를 가져옴 유효한 데이터 컨텍스트가 아니거나 MSTransform유형이 아닌경우 return
            if (!(DataContext is MSTransform vm))
            {
                _undoAction = null;
                _propertyChanged = false;
                return null;
            }

            //변환할 변수 transform.Position에 대해 List의 형태로 값을 변경한다.
            var selection = vm.SelectedComponents.Select(x => selector(x)).ToList();
            return new Action(() =>
            {
                selection.ForEach(x => forEachAction(x));//undoAction은 position값을 이전 값으로 되돌린다.
                (GameEntityView.Instance.DataContext as MSEntity)?.GetMSComponent<MSTransform>().Refresh();//MSTransform 컴포넌트를 새로고침한다.
            });
        }

        private Action GetPositionAction() => GetAction((x) => (x, x.Position), (x) => x.transform.Position = x.Item2);
        private Action GetRotationAction() => GetAction((x) => (x, x.Rotation), (x) => x.transform.Rotation = x.Item2);
        private Action GetScaleAction() => GetAction((x) => (x, x.Scale), (x) => x.transform.Scale = x.Item2);
      
        private void RecordActions(Action redoAction, string name)
        {
            if (_propertyChanged)
            {
                Debug.Assert(_undoAction != null);
                _propertyChanged = false;             
                Project.UndoRedo.Add(new UndoRedoAction(_undoAction, redoAction, name));
            }
        }

        //Position
        private void OnPosition_VectorBox_PreviewMouse_LBD(object sender, MouseButtonEventArgs e)
        {
            _propertyChanged = false;
            _undoAction = GetPositionAction();
        }

        private void OnPosition_VectorBox_PreviewMouse_LBU(object sender, MouseButtonEventArgs e)
        {

            RecordActions(GetPositionAction(), "Position changed");
        }

        //Rotation
        private void OnRotation_VectorBox_PreviewMouse_LBD(object sender, MouseButtonEventArgs e)
        {
            _propertyChanged = false;
            _undoAction = GetRotationAction();
        }

        private void OnRotation_VectorBox_PreviewMouse_LBU(object sender, MouseButtonEventArgs e)
        {

            RecordActions(GetRotationAction(), "Rotation changed");
        }

        //Scale
        private void OnScale_VectorBox_PreviewMouse_LBD(object sender, MouseButtonEventArgs e)
        {
            _propertyChanged = false;
            _undoAction = GetScaleAction();
        }

        private void OnScale_VectorBox_PreviewMouse_LBU(object sender, MouseButtonEventArgs e)
        {

            RecordActions(GetScaleAction(), "Scale changed");
        }

        private void OnPosition_VectorBox_LostKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            if(_propertyChanged && _undoAction != null)
            {
                OnPosition_VectorBox_PreviewMouse_LBU(sender, null);
            }
        }  
        private void OnRotation_VectorBox_LostKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            if(_propertyChanged && _undoAction != null)
            {
                OnRotation_VectorBox_PreviewMouse_LBU(sender, null);
            }
        }  
        private void OnScale_VectorBox_LostKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            if(_propertyChanged && _undoAction != null)
            {
                OnScale_VectorBox_PreviewMouse_LBU(sender, null);
            }
        }
    }
}
