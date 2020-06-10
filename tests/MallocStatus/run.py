from pysys.constants import *
from pysys.basetest import BaseTest
from apama.correlator import CorrelatorHelper

class PySysTest(BaseTest):
	def execute(self):
		for allocator in ['platform', 'jemalloc']:
			corr = CorrelatorHelper(self, name=f'correlator_{allocator}')
			corr.start(logfile=f'correlator_{allocator}.log', environ={'AP_ALLOCATOR':allocator,'AP_TRACKMEMORY':'true'})
			corr.injectEPL(filenames=['ManagementImpl.mon', 'Management.mon'], filedir=self.project.APAMA_HOME+'/monitors')
			corr.injectEPL(filenames=['JEMallocPlugin.mon', 'JEMallocStatus.mon'], filedir=self.input+'/../../../')
			corr.flush()
			corr.manage(['-r', 'malloc_stats'])
			self.wait(12)
			corr.shutdown()

	def validate(self):
		self.assertGrep('correlator_platform.log', expr='Memory Status:.*jeres=.*sys=')
		self.assertGrep('correlator_jemalloc.log', expr='Memory Status:.*sys=')
