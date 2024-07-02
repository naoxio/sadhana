# sadhana_utils.py

import os
import yaml

def read_practice(sadhana_dir, identifier):
    practices_dir = os.path.join(sadhana_dir, "practices")
    
    def find_practice_file(dir):
        for root, dirs, files in os.walk(dir):
            for file in files:
                if file.endswith(".yaml"):
                    full_path = os.path.join(root, file)
                    with open(full_path, 'r') as f:
                        content = f.read()
                        practice = yaml.safe_load(content)
                        if practice and (practice.get('id') == identifier or 
                                         practice.get('short_name') == identifier or 
                                         practice.get('name') == identifier):
                            return practice
        return None
    
    return find_practice_file(practices_dir)

def read_config(config_dir, short_name):
    config_path = os.path.join(config_dir, f"{short_name}_config.yaml")
    if os.path.exists(config_path):
        with open(config_path, 'r') as f:
            return yaml.safe_load(f)
    return None

def write_config(config_dir, short_name, config):
    config_path = os.path.join(config_dir, f"{short_name}_config.yaml")
    
    # Ensure the directory exists
    os.makedirs(config_dir, exist_ok=True)
    
    try:
        with open(config_path, 'w') as f:
            yaml.dump(config, f)
        return True
    except Exception as e:
        print(f"Error writing to file: {str(e)}")
        return False

def generate_default_config(practice):
    config = {}
    
    def process_node(node, config_path):
        if isinstance(node, dict):
            if 'default' in node and 'range' in node:
                key = ".".join(config_path) if config_path else ""
                config[key] = node['default']
            else:
                for k, v in node.items():
                    new_path = config_path + [k]
                    process_node(v, new_path)
    
    process_node(practice.get('exercise_structure', {}), [])
    return config