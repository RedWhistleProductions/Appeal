from pathlib import Path
import json,sys,zipfile
import numpy as np
from PIL import Image,ImageDraw
from scipy.ndimage import label,find_objects,distance_transform_edt
R=Path(__file__).resolve().parent
NN=Image.Resampling.NEAREST
def extract(path):
 a=np.array(Image.open(path).convert('RGBA'));ll,n=label(a[:,:,3]>100);counts=np.bincount(ll.ravel());items=[(i,s) for i,s in enumerate(find_objects(ll),1) if s and counts[i]>500]
 items=sorted(items,key=lambda z:counts[z[0]],reverse=True)[:16];assert len(items)==16,(path,len(items));items.sort(key=lambda z:sum((z[1][0].start,z[1][0].stop)))
 ordered=[]
 for row in range(4):ordered+=sorted(items[row*4:row*4+4],key=lambda z:z[1][1].start)
 out=[]
 for i,s in ordered:
  t=a[s].copy();t[:,:,3]=np.where(ll[s]==i,t[:,:,3],0);t[t[:,:,3]==0,:3]=0;out.append(Image.fromarray(t))
 return out
def metadata(name,size):return {'schema':'bananas.sprite-atlas.v1','image':name,'format':'RGBA8','size':dict(zip(['w','h'],size)),'grid':{'cell_width':64,'cell_height':64,'margin':0,'spacing':0},'coordinate_origin':'top-left','projection':'top-down-oblique','sprites':{}}
def add(d,name,x,y,w=64,h=64,**extra):d['sprites'][name]={'rect':{'x':x,'y':y,'w':w,'h':h},'footprint':{'columns':w//64,'rows':h//64},'pivot':{'x':0,'y':0},**extra}
def save(im,d):
 a=np.array(im);a[a[:,:,3]==0,:3]=0;Image.fromarray(a).save(R/d['image']);(R/d['image'].replace('.png','.json')).write_text(json.dumps(d,indent=2))
tiles=[]
for t in extract(sys.argv[1]):
 inset=max(2,int(min(t.size)*.04));a=np.array(t.crop((inset,inset,t.width-inset,t.height-inset)).resize((64,64),NN));holes=a[:,:,3]<128
 if holes.any():
  idx=distance_transform_edt(holes,return_distances=False,return_indices=True);a[holes]=a[idx[0][holes],idx[1][holes]]
 a[:,:,3]=255;tiles.append(a)
groups=[(0,4),(4,8),(8,12),(12,13),(13,14),(14,15),(15,16)]
for lo,hi in groups:
 g=tiles[lo:hi];lr=np.mean([a[:,3,:3] for a in g]+[a[:,-4,:3] for a in g],axis=0).astype('uint8');tb=np.mean([a[3,:,:3] for a in g]+[a[-4,:,:3] for a in g],axis=0).astype('uint8');corner=np.mean([lr[0],lr[-1],tb[0],tb[-1]],axis=0).astype('uint8');lr[0]=lr[-1]=tb[0]=tb[-1]=corner
 for a in g:
  for k in range(3):
   q=(3-k)/3
   a[:,k,:3]=(a[:,k,:3]*(1-q)+lr*q).astype('uint8');a[:,-1-k,:3]=(a[:,-1-k,:3]*(1-q)+lr*q).astype('uint8');a[k,:,:3]=(a[k,:,:3]*(1-q)+tb*q).astype('uint8');a[-1-k,:,:3]=(a[-1-k,:,:3]*(1-q)+tb*q).astype('uint8')
  a[0,:,:3]=tb;a[-1,:,:3]=tb;a[:,0,:3]=lr;a[:,-1,:3]=lr
names=[f'flagstone_{i}' for i in range(4)]+[f'sandstone_{i}' for i in range(4)]+[f'cave_{i}' for i in range(4)]+['masonry','wood','grating','pit']
im=Image.new('RGBA',(256,256));d=metadata('dungeon_surfaces.png',im.size)
for i,(name,a) in enumerate(zip(names,tiles)):
 x=i%4*64;y=i//4*64;im.paste(Image.fromarray(a),(x,y));add(d,name,x,y,repeat_x=True,repeat_y=True,walkable=name!='pit')
save(im,d)
# A wall network uses N=1,E=2,S=4,W=8 adjacency; transparent outside the wall footprint.
im=Image.new('RGBA',(512,256));d=metadata('dungeon_walls.png',im.size);walltiles={}
for family,ti in [('stone',12),('sandstone',4)]:
 for mask in range(16):
  m=np.zeros((64,64),dtype=bool);m[16:48,16:48]=1
  if mask&1:m[:32,16:48]=1
  if mask&2:m[16:48,32:]=1
  if mask&4:m[32:,16:48]=1
  if mask&8:m[16:48,:32]=1
  a=tiles[ti].copy();a[:,:,3]=m*255
  # Edge shading only on exposed sides; connected edges remain identical.
  for dy,dx,factor in [(0,-1,.50),(0,1,.60),(-1,0,1.18),(1,0,.42)]:
   neighbor=np.roll(m,(dy,dx),(0,1));
   if dy==1:neighbor[0,:]=True
   if dy==-1:neighbor[-1,:]=True
   if dx==1:neighbor[:,0]=True
   if dx==-1:neighbor[:,-1]=True
   edge=m&~neighbor;a[edge,:3]=np.minimum(255,a[edge,:3]*factor).astype('uint8')
  a[~m]=0;t=Image.fromarray(a);walltiles[(family,mask)]=t;x=(mask%8)*64;y=(mask//8+(0 if family=='stone' else 2))*64;im.paste(t,(x,y));add(d,f'{family}_{mask}',x,y,adjacency_mask=mask,collision_mask='alpha',walkable=False)
d['adjacency_rules']={'N':1,'E':2,'S':4,'W':8,'selection':'Sum bits for orthogonally adjacent wall cells. Place above floors. Connected ports are 32 pixels wide.'};save(im,d)
props=extract(sys.argv[2]);namesp=['chest_closed','chest_open','door_closed','door_open','stairs_down','stairs_up','barrel','crate','column','column_broken','rubble','spikes_raised','brazier_unlit','brazier_lit','table','lever']
im=Image.new('RGBA',(512,384));d=metadata('dungeon_props.png',im.size)
for i,(name,t) in enumerate(zip(namesp,props)):
 x=i%8*64;y=i//8*192;h=128 if name in ['door_closed','door_open','column'] else 64;scale=min(60/t.width,(h-4)/t.height);t=t.resize((round(t.width*scale),round(t.height*scale)),NN);im.paste(t,(x+(64-t.width)//2,y+h-2-t.height));add(d,name,x,y,64,h,sort_anchor='pivot');d['sprites'][name]['pivot']={'x':32,'y':h-2}
save(im,d)
# Usable example level, wall masks selected from the map rather than hand placed.
rows=['################','#......#.......#','#......#.......#','#..............#','#......#.......#','###.######.#####','#......#.......#','#..............#','#......#.......#','#......#.......#','#..............#','################']
scene=Image.new('RGBA',(1024,768));cells=[];placements=[]
for y,row in enumerate(rows):
 for x,c in enumerate(row):
  ti=(x*7+y*3)%4;scene.paste(Image.fromarray(tiles[ti]),(x*64,y*64));entry={'x':x,'y':y,'floor':names[ti]}
  if c=='#':
   mask=sum(bit for dx,dy,bit in [(0,-1,1),(1,0,2),(0,1,4),(-1,0,8)] if 0<=y+dy<len(rows) and 0<=x+dx<len(row) and rows[y+dy][x+dx]=='#');t=walltiles[('stone',mask)];scene.alpha_composite(t,(x*64,y*64));entry['wall']=f'stone_{mask}'
  cells.append(entry)
atlas=Image.open(R/'dungeon_props.png')
for name,x,y in [('stairs_up',2,2),('chest_closed',4,3),('barrel',1,7),('crate',2,7),('stairs_down',13,9),('brazier_lit',9,2),('brazier_lit',13,2),('spikes_raised',10,7),('rubble',5,9),('lever',12,7),('table',10,3)]:
 q=d['sprites'][name]['rect'];t=atlas.crop((q['x'],q['y'],q['x']+q['w'],q['y']+q['h']));scene.alpha_composite(t,(x*64,y*64));placements.append({'sprite':name,'x':x,'y':y})
scene.save(R/'example_dungeon.png');(R/'example_map.json').write_text(json.dumps({'tile_size':64,'width':16,'height':12,'cells':cells,'props':placements},indent=2))
checks=0
for lo,hi in groups:
 for a in tiles[lo:hi]:
  for b in tiles[lo:hi]:assert np.array_equal(a[:,-1],b[:,0]) and np.array_equal(a[-1],b[0]);checks+=2
for file in R.glob('dungeon_*.json'):
 dd=json.loads(file.read_text());pic=Image.open(R/dd['image']);occupied=[]
 for e in dd['sprites'].values():
  q=e['rect'];x,y,w,h=[q[k] for k in ['x','y','w','h']];assert all(v%64==0 for v in [x,y,w,h]);assert x+w<=pic.width and y+h<=pic.height;assert pic.crop((x,y,x+w,y+h)).getchannel('A').getbbox()
  for xx,yy,ww,hh in occupied:assert x+w<=xx or xx+ww<=x or y+h<=yy or yy+hh<=y
  occupied.append((x,y,w,h))
(R/'validation.json').write_text(json.dumps({'surfaces':16,'wall_tiles':32,'props':16,'grid':[64,64],'bounds_and_nonoverlap':'passed','surface_edge_checks':checks,'engine_runtime':'not_run'},indent=2))
print('Packed 64 sprites across three atlases; validated grid, bounds, nonoverlap and repeat edges.')
