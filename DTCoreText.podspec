Pod::Spec.new do |spec|
  spec.name         = 'DTCoreText'
  spec.version      = '1.6.23'
  spec.platforms    = {:ios => '9.0', :tvos => '9.0' }
  spec.license      = 'BSD'
  spec.source       = { :git => 'https://github.com/Cocoanetics/DTCoreText.git', :tag => spec.version.to_s }
  spec.source_files = 'Core/Source/*.{h,m,c}'
  spec.ios.source_files = 'Core/Source/iOS/*.{h,m,c}'
  spec.dependency 'DTFoundation/Core', '~>1.7.5'
  spec.dependency 'DTFoundation/UIKit', '~>1.7.5'
  spec.dependency 'DTFoundation/DTHTMLParser', '~>1.7.5'
  spec.dependency 'DTFoundation/DTAnimatedGIF', '~>1.7.5'
  spec.frameworks   = 'MediaPlayer', 'QuartzCore', 'CoreText', 'CoreGraphics', 'ImageIO'
  spec.requires_arc = true
  spec.homepage     = 'https://github.com/Cocoanetics/DTCoreText'
  spec.summary      = 'Methods to allow using HTML code with CoreText.'
  spec.author       = { 'Oliver Drobnik' => 'oliver@cocoanetics.com' }
  spec.documentation_url = 'http://docs.cocoanetics.com/DTCoreText'
  spec.social_media_url = 'https://twitter.com/cocoanetics'
  spec.prefix_header_contents = '#import <CoreText/CoreText.h>'
  spec.xcconfig = { 'GCC_PREPROCESSOR_DEFINITIONS' => '$(inherited)' }
  spec.resource_bundles = { 'Resources': 'Core/Source/default.css' }
  
  spec.default_subspec = 'Core'

  spec.subspec 'Core' do |ap|
    ap.source_files = 'Core/Source/*.{h,m,c}'
  end
  
  spec.subspec 'Extension' do |ap|
    ap.source_files = 'Core/Source/*.{h,m,c}'
    ap.pod_target_xcconfig = { 'GCC_PREPROCESSOR_DEFINITIONS' => '$(inherited) DT_APP_EXTENSIONS=1' }
  end
  
  spec.library = 'iconv'
  spec.vendored_library = 'Core/netsurf/libnetsurf.a'

  netsurf_root = '/Users/yongweihu/Documents/Others/DTCoreText/Core/netsurf'
  spec.pod_target_xcconfig = {
    'NETSURF_SRC_ROOT' => netsurf_root,
    'HEADER_SEARCH_PATHS' => '"$(NETSURF_SRC_ROOT)/include"'
  }
end
