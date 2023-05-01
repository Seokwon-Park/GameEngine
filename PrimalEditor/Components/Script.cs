﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace PrimalEditor.Components
{
    [DataContract]
    class Script : Component
    {
		private string _name;
		[DataMember]
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
        public override IMSComponent GetMultiselectionComponent(MSEntity msEntity) => new MSScript(msEntity);

        public Script(GameEntity owner) :base(owner) { }
    }

	sealed class MSScript : MSComponent<Script>
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

        protected override bool UpdateComponents(string propertyName)
        {
            if(propertyName == nameof(Name))
            {
                SelectedComponents.ForEach(c => c.Name = _name);
                return true;
            }
            return false;
        }

        protected override bool UpdateMSComponent()
        {
            Name = MSEntity.GetMixedValue(SelectedComponents, new Func<Script, string>(x => x.Name));
            return true;
        }

        public MSScript(MSEntity msEntity) : base(msEntity)
        {
            Refresh();
        }        
    }
}
