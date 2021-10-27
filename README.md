<!--
 * @Author: your name
 * @Date: 2021-10-09 14:42:26
 * @LastEditTime: 2021-10-27 14:39:08
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \dialogue-service\README.md
-->
# UTTER

## 项目介绍 

UTTER（**U**nified in**t**elligen**t** dialogu**e** platfo**r**m）

随着AI技术发展和预训练模型逐步壮大，智能对话受到越来越多的技术爱好者和公司青睐，然而要构建一个高可用、稳定性强、功能丰富的对话系统，存在着诸多问题，诸如对话技能设计、领域槽位定义、对话管理、状态跟踪、多轮对话、对话系统架构设计等，每个环节即有技术层面需要解决的问题，又有产品或架构设计层面要考虑的点。
Github也有些比较好的开源对话系统，距离真实产品应用尚有一定距离，基于此，遂创建此仓库，用来做商用级对话平台的研发和学习。



## 项目特点
TO BE DONE!
## 项目架构
TO BE DONE!
## 使用方式
TO BE DONE!


## 研发计划
- [ ] 字符串匹配引擎
  - [x] 优化完全搜索效果，[见代码](https://github.com/syw2014/utter/blob/master/src/StringMatching/ahocorasick.cc#L184)
  
    效果：“我想买个iPhone13promax”，实体词："iphone，iPhone13，iPhone13pro"，
  
    搜索结果：“IPHONE”，“IPHONE13”,"IPHONE13PRO"
  
    注意：默认词典全部转换为大写形式，可在实例化时进行修改
  
  - [x] 优化最长匹配效果
  
    效果：“打开王者王者荣耀”，实体词：“王者”，“王者荣耀”
  
    全词匹配效果：“王者”，“王者”，“王者荣耀”
  
    最长匹配结果：“王者“，”王者荣耀“
  
  - [x] 优化实体词多次出现提取效果
  
  - [x] 代码结构模块化
  
  - [ ] 补充日志模块
- [ ] 对话模板解析引擎
  - [ ] 模板结构设计及解析
  - [ ] 模板召回功能
  - [ ] 模板排序功能

