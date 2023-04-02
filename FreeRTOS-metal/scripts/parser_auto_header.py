#!/usr/bin/env python3

# from string import Template

import pyparsing as pp
import sys
import os
import jinja2

def missingvalue(message):
    """
    Raise an UndefinedError
    This function is made available to the template so that it can report
    when required values are not present and cause template rendering to
    fail.
    """
    raise jinja2.UndefinedError(message)

class AutoHeader (object) :
    """ Class intended to do handle config file templates """
    def __init__(self, input_file_name, output_file_name):
        self.input_file_name = input_file_name
        self.output_file_name = output_file_name

    def generate_header(self, config) :
        """Generate header from template using the config(Config) in parameter

        Args:
            config (Config): Config to use for template rendering

        """
        env = jinja2.Environment(
            loader = jinja2.FileSystemLoader(os.path.dirname(self.input_file_name)),
            trim_blocks=True, lstrip_blocks=True
        )
        template = env.get_template(os.path.basename(self.input_file_name))
        self.output_file.write(template.render(config = config))
    
    def __enter__(self):
        self.output_file = open(self.output_file_name, 'w')
        return(self)

    def __exit__(self, type, value, traceback):
        self.output_file.close()
        del self.output_file

class MakefileConfigBuilder (object) :
    """
    Class intended to process configs comming from Makefile
    
    The goal here is to manage config sequence in order to avoid multiplication
    of makefile variables.
    """
    def __init__(self):
        self.configs = Config()

    def split_configs(self, config_string):
        """ Split config string into config list """
        key = pp.Word(pp.alphanums + "_") ^ pp.Suppress(".")
        groupKey =  pp.Group(pp.OneOrMore(key)).setResultsName("group")
        configValue = pp.Word(pp.alphanums + "_").setResultsName("value")
        config = pp.Group(groupKey + pp.Suppress('=') + configValue)
        configs = pp.OneOrMore(config)
        return(configs.parseString(config_string, parseAll=True))

    def add_configs(self, configs):
        for config in configs :
            self.configs.add_config(config.group, config.value)

class Config (dict, object) :
    """
    Config object
    """

    def add_attr(self, key, value = None):
        if ( key not in self ) :
            if value is None :
                self[key] = Config()
            else :
                self[key] = value
        return(self[key])

    def add_config(self, group, value):
        config = self
        for group_elmt in group[:-1] :
            config = config.add_attr(group_elmt)
        config = config.add_attr(group[-1], value = value)
        return(config)

if __name__ == "__main__" :
    import argparse
    import re
    import os

    parser = argparse.ArgumentParser(description='Template parser using environement variable as keys')

    parser.add_argument('--input_file', 
                        nargs='+',
                        help='specify a Template file',
                        required=True)
    
    parser.add_argument('--output_dir', 
                        nargs=1,
                        help='specify output directory',
                        required=True)

    args = parser.parse_args()

    templateFileNamePattern = re.compile(r".+\.h\.in")

    configBuilder = MakefileConfigBuilder()
    if ( "MAKE_CONFIG" not in os.environ) :
            raise ValueError("Cannot find MAKE_CONFIG in environement variables")
    configBuilder.add_configs(configBuilder.split_configs(os.environ["MAKE_CONFIG"]))

    for template_file in args.input_file :
        # check file name format 
        if(templateFileNamePattern.match(template_file) is None) :
            raise ValueError ("{template_file} doesn't match *.h.in pattern".format(template_file=template_file))

        # remove .in and path 
        output_file = os.path.splitext(os.path.basename(template_file))[0]
        
        output_file = os.path.join(args.output_dir[0], output_file)

        with AutoHeader(template_file, output_file) as config_header:
            config_header.generate_header(configBuilder.configs)

    exit(0)
