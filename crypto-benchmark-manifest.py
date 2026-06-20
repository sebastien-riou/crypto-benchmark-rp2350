class Rp2350:

    @staticmethod
    def sw_targets():
        return ['cortex-m33','rv32imcb']

    def __init__(self):
        self.sw_target = None
    
    def build_cmd(self,sw_target):
        self.sw_target = sw_target
        return {
            'cmd':['./buildit',self.sw_target]
        }
    
    def run_cmd(self,sw_target):
        if self.sw_target != sw_target:
            raise RuntimeError(f'last build was targeting {self.sw_target} but run for {sw_target} is requested')
        return {
            'cmd':['./flash']
        }
    
helper = Rp2350()