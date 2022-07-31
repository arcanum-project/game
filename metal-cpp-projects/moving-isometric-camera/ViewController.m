//
//  ViewController.m
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#import "ViewController.h"
#import "MetalKit/MTKView.h"
#import "RendererDelegateAdapter.h"

@interface ViewController ()

@end

@implementation ViewController
{
  MTKView * _pView;
  RendererDelegateAdapter * _pRendererDelegateAdapter;
}
- (void)viewDidLoad {
	[super viewDidLoad];
	// Do any additional setup after loading the view.
  
  _pView = (MTKView *)self.view;
  _pView.device = MTLCreateSystemDefaultDevice();
  if (!_pView.device)
  {
	NSLog(@"Metal is not supported on this device");
	self.view = [[UIView alloc] initWithFrame:self.view.frame];
	return;
  }
  _pView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
  _pView.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
  _pRendererDelegateAdapter = [[RendererDelegateAdapter alloc] initWithMetalKitView:_pView];
  [_pRendererDelegateAdapter mtkView:_pView drawableSizeWillChange:_pView.bounds.size];
  _pView.delegate = _pRendererDelegateAdapter;
}

@end
