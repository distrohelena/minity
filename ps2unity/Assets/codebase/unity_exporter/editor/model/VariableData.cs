using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ps2unity {
    public class VariableData {
        public string Name { get; set; }
        public string Type { get; set; }
        public string Value { get; set; }
        public string Getter { get; set; }
        public string Setter { get; set; }

        public VariableData() {

        }
        public VariableData(string name, string type, string value, string getter = "", string setter = "") {
            Name = name;
            Type = type;
            Value = value;
            Getter = getter;
            Setter = setter;
        }

        public override string ToString()
        {
            return Name;
        }
    }
}
