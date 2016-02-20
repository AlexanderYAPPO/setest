#!/usr/bin/python

import os.path
import argparse
import os
from string import *

from lxml import etree as ETree

desc = 'By given path, scanning all .xquery files in given folder, and converting \
them to usable format, creating appropriate structure in current dir'

class StringException(Exception):
    
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return self.value

class Testset:
    def __init__(self, name):
        self.name = name
        if os.path.exists(self.name):
            raise StringException('Testset directory already exists')
        
        os.mkdir(self.name)
        
        self.root = ETree.Element('TestGroup', {'name': self.name})
        self.etree = ETree.ElementTree(self.root)

    def add_test(self, testfile):
        '''Given testfile should have .xquery suffix'''
        '''Now only ampersand problems are handled'''
        testname = os.path.split(testfile)[-1].split('.')[0]
        
        f = open(testfile);
        tests = f.read()
        tests = tests.split('&')
        f.close()
        
        attrs = {'name': testname, 'filepath': os.path.join(self.name, testname)}
        testnode = ETree.SubElement(self.root, 'TestCase', attrs)
        testfiles = ETree.SubElement(testnode, 'InputFiles');
        
        os.makedirs(os.path.join(self.name, testname))
        for i in xrange(len(tests)):
            if len(tests[i]) > 0:
                '''Here we should determine if the query performs bulk load 
                and parse filename and dbname in this case'''
                
                if (lower(tests[i]).find('load') != -1):
                    '''Bulkload query'''
                    '''Assuming that " symbols are presented only in source 
                        filename and docname'''
                    load_query = True
                        
                    ind1 = find(tests[i], '"')
                    ind2 = ind1 + 1 + find(tests[i][ind1 + 1:], '"')
                    ind3 = ind2 + 1 + find(tests[i][ind2 + 1:], '"')
                    ind4 = ind3 + 1 + find(tests[i][ind3 + 1:], '"')
                                        
                    print tests[i]
                    print ind1, ind2, ind3, ind4
                                        
                    source_file = tests[i][ind1 + 1:ind2]
                    source_file = replace(source_file, '../cdb/', '')
                    docname = tests[i][ind3 + 1:ind4]
                    attrs = {'QueryType': 'LOAD', 'SourceFile': source_file, 'Document': docname}
                    
                    '''In case of loading into collection colname also required'''
                    ind5 = ind4 + 1 + find(tests[i][ind4 + 1:], '"')
                    if (ind5 != ind4):
                        ind6 = ind5 + 1 + find(tests[i][ind5 + 1:], '"')
                        colname = tests[i][ind5 + 1:ind6]
                        attrs['Collection'] = colname
                    
                    testfile = ETree.SubElement(testfiles, 'InputFile', attrs)
                else:
                    attrs = {'QueryType': 'USUAL'}
                    testfile = ETree.SubElement(testfiles, 'InputFile', attrs)

                    filename = testname + '-' + str(i) + '.xquery'
                    testfile.text = filename

                    f = open(os.path.join(self.name, testname, filename), 'w')
                    f.write(tests[i])
                    f.close()

    def dump_config(self):
        '''Writes the test group configuration file'''
        self.etree.write(os.path.join(self.name, self.name + '.xml'), pretty_print=True)
        
def iterate_tests(path, testset):
    files = os.listdir(path)
    files.sort()
    for entry in files:
        if entry[-7:] == '.xquery':
            testset.add_test(os.path.join(path, entry))

def main():
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument('testset_name')
    parser.add_argument('tests_dir')
    args = vars(parser.parse_args())

    testset = Testset(args['testset_name'])
    iterate_tests(args['tests_dir'], testset)
    testset.dump_config()
    
main()
